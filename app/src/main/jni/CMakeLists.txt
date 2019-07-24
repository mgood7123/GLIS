cmake_minimum_required(VERSION 3.4.1)

add_library(nativeegl SHARED jniapi.cpp renderer.cpp)
target_link_libraries(nativeegl Android log EGL GLESv3)