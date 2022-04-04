cmake_minimum_required(VERSION 3.18)
project(project-chat VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
# set(CMAKE_CXX_EXTENSIONS true)

find_package(Threads REQUIRED)

add_definitions("-DASIO_STANDALONE")
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE asio/asio/include)
target_link_libraries(asio INTERFACE Threads::Threads)

set(CONNECTIONLIB_SOURCE connection.h connection.cpp)
add_library(connectionlib STATIC ${CONNECTIONLIB_SOURCE})
target_link_libraries(connectionlib PUBLIC asio)

add_executable(chat_client client.cpp)
target_link_libraries(chat_client PRIVATE connectionlib)

add_executable(chat_server server.cpp)
target_link_libraries(chat_server PRIVATE connectionlib)

# Examples
add_executable(daytime_server "examples/tcp_daytime_server.cpp")
target_link_libraries(daytime_server PRIVATE asio Threads::Threads)

add_executable(daytime_client "examples/tcp_daytime_client.cpp")
target_link_libraries(daytime_client PRIVATE asio Threads::Threads)