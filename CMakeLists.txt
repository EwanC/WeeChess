project(WeeChess)
cmake_minimum_required(VERSION 2.8)

IF(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
message( FATAL_ERROR "Only Linux currently supported." )
endif()

set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("include")
add_subdirectory(source)
