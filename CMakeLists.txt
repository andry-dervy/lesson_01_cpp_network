cmake_minimum_required(VERSION 3.5)

project(lesson_01_cpp_network LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_byte_order test_byte_order.cpp)

add_subdirectory(resolve)
add_subdirectory(socket_wrapper)
add_subdirectory(udp_server)
add_subdirectory(udp_client)
