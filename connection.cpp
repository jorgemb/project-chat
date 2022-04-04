#include "connection.h"
#include "connection.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

TcpConnection::pointer TcpConnection::create(asio::io_context& context, const asio::ip::tcp::endpoint & endpoint) {
	// Create connected socket
	asio::ip::tcp::socket socket(context);
	socket.connect(endpoint);

	return pointer(new TcpConnection(std::move(socket)));
}

TcpConnection::pointer TcpConnection::create(asio::io_context& context, const asio::ip::tcp::resolver::results_type& endpoints) {
	// Create connected socket
	asio::ip::tcp::socket socket(context);
	asio::connect(socket, endpoints);

	return pointer(new TcpConnection(std::move(socket)));
}

TcpConnection::pointer TcpConnection::create(asio::ip::tcp::socket&& socket) {
	return pointer(new TcpConnection(std::move(socket)));
}

void TcpConnection::write_async(const std::string& message)
{
	pointer self(shared_from_this());
	auto message_data = std::make_shared<std::string>(message);

	m_socket.async_write_some(asio::buffer(message_data->c_str(), message_data->size()),
		[this, self, message_data](const asio::error_code& error, std::size_t written) {
		}
	);
}


void TcpConnection::start_reading_async(std::function<void(std::string&& msg)> handler){
	using namespace std::placeholders;

	auto self = shared_from_this();
	m_socket.async_read_some(
		asio::buffer(m_buffer),
		[this, self, handler](const asio::error_code& error, std::size_t bytes) {
			if (!error && bytes > 0) {
				std::string message(m_buffer.begin(), m_buffer.begin() + bytes);

				// DEBUG
				//std::cout << "Received: (" << message << ")" << std::endl;

				// Send message to handler
				if (handler) {
					handler(std::move(message));
				}
				
				// Continue loop
				start_reading_async(handler);
			}
			else if (error) {
				std::cerr << "Connection error: (" << error.value() << ") - " << error.message() << std::endl;
				m_socket.close();
			}

		}
	);
}

void TcpConnection::close() {
	m_socket.close();
}

bool TcpConnection::is_connected() const {
	return m_socket.is_open();
}

TcpConnection::~TcpConnection() {
}

TcpConnection::TcpConnection(asio::ip::tcp::socket&& socket) : m_socket(std::move(socket)), m_buffer{} {
}


TcpServer::TcpServer(asio::io_context& context, asio::ip::port_type port, asio::ip::tcp listen_interface):
m_context(context), m_acceptor(context, asio::ip::tcp::endpoint(listen_interface, port)), m_continue(true), m_next_id(0){
	start_accepting();
}

void TcpServer::stop_server() {
	m_continue = false;
	m_acceptor.close();

	// Cancel each connection
	std::lock_guard lock(m_connections_mutex);
	for (auto [id,conn]: m_connections) {
		conn->close();
	}
}

void TcpServer::start_accepting() {
	auto socket_ptr = std::make_shared<asio::ip::tcp::socket>(m_context);

	m_acceptor.async_accept(
		*socket_ptr,
		[this, socket_ptr](auto error) {
			if (!error) {
				// DEBUG
				std::cout << "Accepted connection from: " << socket_ptr->remote_endpoint() << std::endl;

				auto connection = TcpConnection::create(std::move(*socket_ptr));
				IdType client_id{};
				{
					std::lock_guard lock(m_connections_mutex);
					client_id = m_next_id;
					m_connections[client_id] = connection;

					++m_next_id;
				}

				connection->start_reading_async([this, client_id](std::string&& message) {
					std::stringstream final_message;
					final_message << client_id << ":" << std::move(message);

					// DEBUG
					std::cout << final_message.str() << std::endl;

					resend_message(client_id, final_message.str());
				});

				// Continue loop
				if(m_continue) start_accepting();
			}
			else {
				std::cerr << "SERVER ERROR: " << error.message() << std::endl;
			}

		}
	);
}

void TcpServer::resend_message(IdType sender, const std::string& message) {
	std::lock_guard lock(m_connections_mutex);
	for (auto iter = m_connections.begin(); iter != m_connections.end(); ) {
		if (iter->second->is_connected()) {
			// Send message to client
			if (iter->first != sender) {
				iter->second->write_async(message);
			}
			++iter;
		}
		else {
			// Remove client if it is not connected
			iter = m_connections.erase(iter);
		}
	}
}


