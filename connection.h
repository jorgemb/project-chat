#ifndef GUARD_CONNECTION_H
#define GUARD_CONNECTION_H

#include <asio.hpp>
#include <memory>
#include <string>
#include <optional>
#include <mutex>
#include <map>
#include <functional>

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	using pointer = std::shared_ptr<TcpConnection>;

	/// <summary>
	/// Creates a new TcpConnection that connects to an endpoint.
	/// </summary>
	/// <param name="context"></param>
	/// <returns></returns>
	[[ nodiscard ]]
	static pointer create(asio::io_context& context, const asio::ip::tcp::endpoint &endpoint);


	/// <summary>
	/// Create a connection form a resolver result
	/// </summary>
	/// <param name="context"></param>
	/// <param name="endpoints"></param>
	/// <returns></returns>
	[[ nodiscard ]]
	static pointer create(asio::io_context& context, const asio::ip::tcp::resolver::results_type& endpoints);

	/// <summary>
	/// Create a new TcpConnection from a socket
	/// </summary>
	/// <param name="socket"></param>
	/// <returns></returns>
	[[ nodiscard ]]
	static pointer create(asio::ip::tcp::socket&& socket);

	/// <summary>
	/// Writes a new message asynchronously
	/// </summary>
	/// <param name="message"></param>
	void write_async(const std::string& message);

	/// <summary>
	/// Starts the reading loop
	/// </summary>
	void start_reading_async(std::function<void(std::string&& msg)> handler);

	/// <summary>
	/// Stops the reading loop on the next opportunity
	/// </summary>
	void close();

	/// <summary>
	/// Returns true if the socket is connected
	/// </summary>
	/// <returns></returns>
	bool is_connected() const;

	/// <summary>
	/// Default virtual destructor
	/// </summary>
	virtual ~TcpConnection();
private:
	/// <summary>
	/// Create a connection using the given socket
	/// </summary>
	/// <param name="socket"></param>
	TcpConnection(asio::ip::tcp::socket&& socket);

	asio::ip::tcp::socket m_socket;
	std::array<char, 1024> m_buffer;
};

class TcpServer{
public:
	/// <summary>
	/// Create a new listen server
	/// </summary>
	/// <param name="port"></param>
	/// <param name="listen_address"></param>
	TcpServer(asio::io_context& context, asio::ip::port_type port, asio::ip::tcp listen_interface = asio::ip::tcp::v4());

	/// <summary>
	/// Stops the server
	/// </summary>
	void stop_server();

private:
	using IdType = unsigned int;

	/// <summary>
	/// Start the accepting loop
	/// </summary>
	void start_accepting();

	/// <summary>
	/// Resends a received message to all the clients
	/// </summary>
	/// <param name="sender"></param>
	/// <param name="message"></param>
	void resend_message(IdType sender, const std::string& message);

	asio::io_context &m_context;
	asio::ip::tcp::acceptor m_acceptor;
	bool m_continue;

	// Clients
	std::mutex m_connections_mutex;
	std::map<IdType, TcpConnection::pointer> m_connections;
	IdType m_next_id;
};

#endif // !GUARD_CONNECTION_H
