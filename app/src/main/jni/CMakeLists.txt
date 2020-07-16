cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(GLIS)
include_directories(${GLIS_INCLUDE_DIRECTORIES})

add_library(nativeegl SHARED compositor.cpp)
target_link_libraries(nativeegl GLIS)