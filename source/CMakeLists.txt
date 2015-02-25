set(EXE_NAME WeeChess)
set(BUILD_DIR ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Src files.
add_executable(${EXE_NAME}
               main.cpp
               board.cpp
               move.cpp
               perft.cpp
               search.cpp
)
