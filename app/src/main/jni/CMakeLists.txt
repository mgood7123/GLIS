cmake_minimum_required(VERSION 3.4.1)
add_library(nativeegl SHARED compositor.cpp)
target_link_libraries(nativeegl android log EGL GLESv3)

add_executable(MYPRIVATEAPP MYPRIVATEAPP.cpp)
target_link_libraries(MYPRIVATEAPP log EGL GLESv3)
add_custom_command(
        TARGET MYPRIVATEAPP
        POST_BUILD
        COMMAND echo "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}"
        COMMAND echo "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}"
        COMMAND echo "CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}"
        COMMAND cp -v MYPRIVATEAPP ${CMAKE_SOURCE_DIR}/executables
)
