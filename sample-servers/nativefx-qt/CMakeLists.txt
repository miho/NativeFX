cmake_minimum_required(VERSION 3.9)

project(nativefx-qt)

# this project relies on C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets WebEngine WebEngineWidgets REQUIRED)

include_directories("../ext/headers/")
include_directories("../../src/main/native/ext/headers/")
include_directories("../headers/")
include_directories("./src/main/headers/")

set(SOURCES
./src/main/cpp/main.cpp
./src/main/cpp/vfxwebpage.cpp
./src/main/cpp/__macos__disable_appnap.cpp
./src/main/headers/vfxwebpage.h
)

# this whole section is just to force macos not to put our server into 
# appnap mode
#
# we treat the __macos__disable_appnap.cpp as mm file (objective-cpp)
# and tell the os to leave us alone
if(APPLE)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-framework Cocoa")
    set( TREAT_AS_OBJCPP "-x objective-c++")
    foreach(SRC ${SOURCES})
        if(${SRC} MATCHES __macos__disable_appnap.cpp$)
            set_source_files_properties(
                ${CMAKE_CURRENT_SOURCE_DIR}/${SRC} 
                PROPERTIES 
                COMPILE_FLAGS 
                "${TREAT_AS_OBJCPP}")
        endif()
    endforeach(SRC)
endif()

add_executable(nativefx-qt ${SOURCES})

# todo: is there a better way to detect linux?
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    target_link_libraries(nativefx-qt Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets rt pthread)
else()
    target_link_libraries(nativefx-qt Qt5::Widgets Qt5::WebEngine Qt5::WebEngineWidgets )
endif()

