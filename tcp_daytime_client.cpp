#include <iostream>
#include <array>
#include <string>
#include <asio.hpp>

using asio::ip::tcp;

int main(int argc, char* argv[]){
    try{
        if(argc != 2){
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

        for(auto& e: endpoints){
            std::cout << e.host_name() << ":" << e.service_name() << std::endl;
        }


        for(;;){
            tcp::socket socket(io_context);
            asio::connect(socket, endpoints);

            std::array<char, 128> buf;
            // std::string buf(128, ' ');
            // std::vector<char> buf(128);
            asio::error_code error;

            size_t len = socket.read_some(asio::buffer(buf), error);
            if(error == asio::error::eof){
                break;
            }else if(error){
                throw asio::system_error(error);
            }

            if(len > 0){
                std::cout << "Received: ";
                std::cout.write(buf.data(), len);
                std::cout << std::endl;
            }
        }

    }catch(const std::exception& e){
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
}