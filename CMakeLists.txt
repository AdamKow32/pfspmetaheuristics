cmake_minimum_required(VERSION 3.10)
project(PFSP_Evolutionary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES
        "src/*.cpp"
        "main.cpp"
)

file(GLOB HEADERS
        "include/*.hpp"
        "include/*.h"
)

include_directories(include)

add_executable(PFSP_Solver ${SOURCES} ${HEADERS})

if(MSVC)
    target_compile_options(PFSP_Solver PRIVATE /W4 /O2)
else()
    target_compile_options(PFSP_Solver PRIVATE -Wall -O3)
endif()