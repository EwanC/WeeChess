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

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("Debug Mode")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -std=c++11 -Wno-write-strings")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
ELSE(CMAKE_BUILD_TYPE MATCHES DEBUG)
    message("Release Mode")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -std=c++11 -Wno-write-strings")
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

SET(SRC_DIR "${CMAKE_SOURCE_DIR}/source")
MESSAGE( STATUS "source dir:" ${SRC_DIR} )
add_definitions(-D__DIR__=${SRC_DIR})

include_directories("include")
add_subdirectory(source)