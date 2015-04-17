project(WeeChess)
cmake_minimum_required(VERSION 2.8)

IF(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
message( FATAL_ERROR "Only Linux currently supported." )
endif()

find_library(OCL_LIBRARY NAMES OpenCL PATHS /usr/local/cuda/lib)
include_directories(/usr/local/cuda/include)

IF(NOT OCL_LIBRARY)
message(FATAL_ERROR "Need OpenCL Library")
endif(NOT OCL_LIBRARY)

set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories("include")
add_subdirectory(source)

