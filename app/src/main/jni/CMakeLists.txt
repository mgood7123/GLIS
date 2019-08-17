cmake_minimum_required(VERSION 3.4.1)
add_subdirectory(WINAPI)
add_library(nativeegl SHARED compositor.cpp)
target_link_libraries(nativeegl android log EGL GLESv3 WinKernel)

add_executable(MYPRIVATEAPP compositor_examples/MYPRIVATEAPP.cpp)
target_link_libraries(MYPRIVATEAPP log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MYPRIVATEAPP
        POST_BUILD
        COMMAND (mkdir \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\" || true)
        COMMAND cp -v MYPRIVATEAPP \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(MovingWindows compositor_examples/MovingWindows.cpp)
target_link_libraries(MovingWindows log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MovingWindows
        POST_BUILD
        COMMAND (mkdir \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\" || true)
        COMMAND cp -v MovingWindows \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)
