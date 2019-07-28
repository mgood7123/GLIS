cmake_minimum_required(VERSION 3.4.1)
add_library(nativeegl SHARED compositor.cpp)
target_link_libraries(nativeegl android log EGL GLESv3)