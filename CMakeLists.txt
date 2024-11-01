cmake_minimum_required(VERSION 3.30)
project(Xanadu)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

include_directories(src)

# Library

add_library(xanadu STATIC
    src/XVDTypes.cpp
    src/XanaduXVD.cpp
)

# CLI

add_executable(XanaduCLI
    XanaduCLI/XanaduCLI.cpp
)

target_link_libraries(XanaduCLI PUBLIC xanadu)