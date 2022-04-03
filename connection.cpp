#include "connection.h"
#include <stdexcept>

// DEBUG
#include <iostream>

void TcpConnectionBase::write_async(const std::string& message)
{
	pointer self(shared_from_this());
	auto message_data = std::make_shared<std::string>(message);

	m_socket.async_write_some(asio::buffer(message_data->c_str(), message_data->size()),
		[this, self, message_data] (const asio::error_code& error, std::size_t written){
		}
	);
}

TcpConnectionBase::~TcpConnectionBase()
{
}

TcpConnectionBase::pointer TcpConnectionBase::create(asio::io_context& context) {
	return pointer(new TcpConnectionBase(context));
}

void TcpConnectionBase::start_reading_async() {
	using namespace std::placeholders;

	auto self = shared_from_this();
	m_socket.async_read_some(
		asio::buffer(m_buffer),
		[this, self](const asio::error_code& error, std::size_t bytes) {
			if (!error) {
				m_messages.emplace_back(m_buffer.begin(), m_buffer.begin() + bytes);

				// DEBUG
				std::cout << "Received: (" << m_messages.back() << ")" << std::endl;
				start_reading_async();
			}
			else {
				std::cerr << "Connection error:" << error.message() << std::endl;
			}
			
		}
	);
}

void TcpConnectionBase::close() {
	m_socket.close();
}

TcpConnectionBase::TcpConnectionBase(asio::io_context& context): m_socket(context) {
}

TcpConnection::pointer TcpConnection::create(asio::io_context& context, const asio::ip::tcp::endpoint& endpoint)
{
	return pointer(new TcpConnection(context, endpoint));
}

TcpConnection::TcpConnection(asio::io_context& context, const asio::ip::tcp::endpoint& endpoint): TcpConnectionBase(context) {
	m_socket.connect(endpoint);
}
