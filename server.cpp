#include "connection.h"

#include <iostream>
#include <future>
#include <asio.hpp>

int main(int argc, char* argv[]) {
	// Verify arguments
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
		return 0;
	}
	unsigned int port{};
	try
	{
		port = std::stoi(argv[1]);
	}
	catch (const std::exception&)
	{
		std::cerr << "Invalid number: " << argv[1] << std::endl;
		return -1;
	}

	// Start listening for connections
	asio::io_context context;
	TcpServer server(context, 5555);

	// Register for signal termination
	asio::signal_set signals(context, SIGINT, SIGTERM);
	signals.async_wait([&server](auto error, int signal) {
		if (!error) {
			if (signal == SIGINT) std::cout << "Ctrl+C received... exiting" << std::endl;
			else if (signal == SIGTERM) std::cout << "SIGTERM received... exiting" << std::endl;

			server.stop_server();
		}
		});

	context.run();
	// auto f1 = std::async(std::launch::async, [&context]() { context.run(); });
	// auto f2 = std::async(std::launch::async, [&context]() { context.run(); });
	// auto f3 = std::async(std::launch::async, [&context]() { context.run(); });
	// auto f4 = std::async(std::launch::async, [&context]() { context.run(); });

	// std::cin.get();
	// server.stop_server();
}
