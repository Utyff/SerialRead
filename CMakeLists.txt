cmake_minimum_required(VERSION 3.6)
project(SerialRead)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Serial.cpp serialread.h)
add_executable(SerialRead ${SOURCE_FILES})
target_link_libraries(SerialRead libwinmm.a)
