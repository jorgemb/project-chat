#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <memory>
#include <asio.hpp>

using asio::ip::tcp;

std::string make_daytime_string(){
    using clock = std::chrono::system_clock;

    auto now = clock::to_time_t(clock::now());
    return ctime(&now);
}

class tcp_connection: public std::enable_shared_from_this<tcp_connection>{
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    [[ nodiscard ]]
    static pointer create(asio::io_context& io_context){
        return pointer(new tcp_connection(io_context));
    }

    tcp::socket& socket(){
        return socket_;
    }

    void start(){
        message_ = make_daytime_string();
        asio::async_write(socket_, asio::buffer(message_),
            std::bind(&tcp_connection::handle_write, shared_from_this(),
            std::placeholders::_1, std::placeholders::_2)
        );
    }

    // ~tcp_connection(){
    //     std::cout << "Connection closed from: " << socket_.remote_endpoint().port() << std::endl;
    // }

private:
    tcp::socket socket_;
    std::string message_;

    tcp_connection(asio::io_context& io_context): socket_(io_context){}

    void handle_write(const asio::error_code& e, size_t bytes_transfered){
    }
};

class tcp_server{
public:
    tcp_server(asio::io_context& io_context)
    : io_context_(io_context), 
    acceptor_(io_context, tcp::endpoint(tcp::v4(), 13)) 
    {
        start_accept();
    }
private:
    asio::io_context& io_context_;
    tcp::acceptor acceptor_;

    void start_accept(){
        tcp_connection::pointer new_connection =
            tcp_connection::create(io_context_);
        
        acceptor_.async_accept(new_connection->socket(),
            std::bind(&tcp_server::handle_accept, this, new_connection,
            std::placeholders::_1)
        );
    }

    void handle_accept(tcp_connection::pointer new_connection, const asio::error_code& error){
        if(!error){
            new_connection->start();
        }
        std::cout << "[" << std::this_thread::get_id() << "] New connection from: " << new_connection->socket().remote_endpoint().port() << std::endl;

        start_accept();
    }
};



int main(){
    try{
        asio::io_context context;
        tcp_server server(context);

         //auto f1 = std::async(std::launch::async, [&](){ context.run(); });
         //auto f2 = std::async(std::launch::async, [&](){ context.run(); });
         //auto f3 = std::async(std::launch::async, [&](){ context.run(); });
        context.run();
    }catch(const std::exception& e){
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }

    return 0;
}