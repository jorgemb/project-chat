#include <iostream>
#include <tuple>
#include <string>
#include <chrono>
#include <future>
#include <asio.hpp>

#include "connection.h"

int main(int argc, char* argv[]) {
	using namespace std::chrono_literals;

	// Get command line arguments
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
		return 0;
	}
	std::string host(argv[1]);
	std::string port(argv[2]);

	// Start asio context
	try {
		asio::io_context context;

		asio::ip::tcp::resolver resolver(context);
		auto endpoints = resolver.resolve(host, port);

		auto connection = TcpConnection::create(context, endpoints);

		auto buffer = std::make_shared <std::array<char, 1024>>();
		connection->start_reading_async([buffer](auto message) {
			// Get all input from stdin
			std::cout << std::move(message) << std::endl;
		});

		// Handle signals
		asio::signal_set signals(context, SIGINT, SIGTERM);
		signals.async_wait([connection](auto error, int signal){
			if(!error){
				if (signal == SIGINT) std::cout << "Ctrl+C received... exiting" << std::endl;
				else if (signal == SIGTERM) std::cout << "SIGTERM received... exiting" << std::endl;

				connection->close();
			}
		});

		// Start running in another process
		auto f = std::async(std::launch::async, [&context]() { context.run(); });

		while (std::cin && connection->is_connected()) {
			std::string message;
			std::cout << "> ";
			std::getline(std::cin, message);

			if (!message.empty()) {
				connection->write_async(message);
			}
		}
		connection->close();
	}
	catch (const std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
	}

	return 0;
}