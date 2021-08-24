# at least cmake 3.9 is required to build this project
cmake_minimum_required(VERSION 3.14)

project(sample-server)

# this project relies on C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# library sources
set(SOURCES		
    src/main/cpp/main.cpp
)

set(CMAKE_BUILD_TYPE DEBUG)

include_directories("../ext/headers/")
include_directories("../../src/main/native/ext/headers/")
include_directories("../headers/")
include_directories("./src/main/headers/")

# create an executable
add_executable(sample-server ${SOURCES})

# todo: is there a better way to detect linux?
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    target_link_libraries(sample-server rt pthread)
endif()

#target_link_libraries (nativefx libname)

