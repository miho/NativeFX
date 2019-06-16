# at least cmake 3.10 is required to build this project
cmake_minimum_required(VERSION 3.10)

project(nativefx)

# this project relies on C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# library sources
set(SOURCES		
    src/main/cpp/nativefx.cpp
    src/main/cpp/jnitypeconverter.cpp
)

set(CMAKE_BUILD_TYPE Release)
find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
include_directories("src/main/headers/")

# create a shared library from the sources and link it against libs.
add_library(nativefx SHARED ${SOURCES})
target_link_libraries(nativefx ${JNI_LIBRARIES})
#target_link_libraries (nativefx libname)
