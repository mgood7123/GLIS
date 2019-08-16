cmake_minimum_required(VERSION 3.4.1)
add_subdirectory(WINAPI)
add_library(nativeegl SHARED compositor.cpp)
target_link_libraries(nativeegl android log EGL GLESv3 WinKernel)

add_executable(MYPRIVATEAPP MYPRIVATEAPP.cpp)
target_link_libraries(MYPRIVATEAPP log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MYPRIVATEAPP
        POST_BUILD
        COMMAND echo "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}"
        COMMAND echo "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}"
        COMMAND echo "CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}"
        COMMAND echo "CMAKE_ANDROID_ARCH_ABI = ${CMAKE_ANDROID_ARCH_ABI}"
        COMMAND (mkdir \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\" || true)
        COMMAND cp -v MYPRIVATEAPP \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)
