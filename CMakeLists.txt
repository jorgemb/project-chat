cmake_minimum_required(VERSION 3.18)
project(project-chat VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
# set(CMAKE_CXX_EXTENSIONS true)

# Add conan packages
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Add other packages
find_package(Threads REQUIRED)

add_definitions("-DASIO_STANDALONE")
add_library(asio INTERFACE)
target_link_libraries(asio INTERFACE Threads::Threads CONAN_PKG::asio)

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

add_executable(example_server "examples/chat_server.cpp" "examples/chat_message.hpp")
target_link_libraries(example_server PRIVATE asio Threads::Threads)

add_executable(example_client "examples/posix_chat_client.cpp" "examples/chat_message.hpp")
target_link_libraries(example_client PRIVATE asio Threads::Threads)
