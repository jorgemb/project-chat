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
	asio::io_context context;
	auto connection = TcpConnection::create(context, asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(host), 5555));
	connection->start_reading_async();

	asio::steady_timer timer(context, 10s);
	timer.async_wait([connection](auto error) { connection->close(); });

	auto f = std::async(std::launch::async, [&context]() { context.run(); });

	while (std::cin) {
		std::string message;
		std::cout << "Message: ";
		std::cin >> message;

		connection->write_async(message);
	}

	return 0;
}