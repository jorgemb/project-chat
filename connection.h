#ifndef GUARD_CONNECTION_H
#define GUARD_CONNECTION_H

#include <asio.hpp>
#include <memory>
#include <deque>
#include <string>
#include <optional>
#include <mutex>

class TcpConnectionBase : public std::enable_shared_from_this<TcpConnectionBase> {
public:
	using pointer = std::shared_ptr<TcpConnectionBase>;

	void write_async(const std::string& message);
	//TcpConnectionBase& operator>>(std::string& output);

	virtual ~TcpConnectionBase();

	/// <summary>
	/// Starts the reading loop
	/// </summary>
	void start_reading_async();

	/// <summary>
	/// Stops the reading loop on the next opportunity
	/// </summary>
	void close();
protected:
	asio::ip::tcp::socket m_socket;

	/// <summary>
	/// Creates a new TcpConnectionBase from context.
	/// </summary>
	/// <param name="context"></param>
	/// <returns></returns>
	static pointer create(asio::io_context& context);

	TcpConnectionBase(asio::io_context& context);
private:
	std::deque<std::string> m_messages;
	std::mutex m_messages_mutex;

	std::array<char, 1024> m_buffer;

	bool m_keep_reading;
};

class TcpConnection : public TcpConnectionBase {
public:
	using pointer = std::shared_ptr<TcpConnection>;

	static pointer create(asio::io_context& context, const asio::ip::tcp::endpoint& endpoint);

private:
	TcpConnection(asio::io_context& context, const asio::ip::tcp::endpoint& endpoint);

};

#endif // !GUARD_CONNECTION_H
