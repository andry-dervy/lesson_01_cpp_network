cmake_minimum_required(VERSION 3.10)

project(dns-resolver C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(${PROJECT_NAME}_SRC
    main.cpp
    resolver.h
    resolver.cpp)

source_group(source FILES ${${PROJECT_NAME}_SRC})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")

include_directories(../socket-wrapper)
target_link_libraries("${PROJECT_NAME}" socket-wrapper)

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
endif()
