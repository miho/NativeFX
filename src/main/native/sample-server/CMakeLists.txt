# at least cmake 3.10 is required to build this project
cmake_minimum_required(VERSION 3.9)

project(sample-server)

# this project relies on C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# library sources
set(SOURCES		
    src/main/cpp/main.cpp
)

set(CMAKE_BUILD_TYPE RELEASE)

include_directories("../src/main/headers/")
include_directories("../ext/headers/")
include_directories("./ext/headers/")

# create a executable
add_executable(sample-server ${SOURCES})
#target_link_libraries(nativefx ${JNI_LIBRARIES})
#target_link_libraries (nativefx libname)
