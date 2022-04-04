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
	auto f = std::async(std::launch::async, [&context]() { context.run(); });

	std::cin.get();
	server.stop_server();
}
