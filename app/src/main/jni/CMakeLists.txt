cmake_minimum_required(VERSION 3.4.1)
add_subdirectory(WINAPI)
add_library(nativeegl SHARED compositor.cpp shm.cpp ashmem.cpp)
target_link_libraries(nativeegl android log EGL GLESv3 WinKernel)

add_executable(MYPRIVATEAPP compositor_examples/MYPRIVATEAPP.cpp)
target_link_libraries(MYPRIVATEAPP log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MYPRIVATEAPP
        POST_BUILD
        COMMAND cp -v MYPRIVATEAPP \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(MovingWindows compositor_examples/MovingWindows.cpp)
target_link_libraries(MovingWindows log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MovingWindows
        POST_BUILD
        COMMAND cp -v MovingWindows \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(DefaultFramebuffer compositor_examples/DefaultFramebuffer.cpp)
target_link_libraries(DefaultFramebuffer log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET DefaultFramebuffer
        POST_BUILD
        COMMAND cp -v DefaultFramebuffer \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(shm compositor_examples/shm.cpp)
target_link_libraries(shm log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET shm
        POST_BUILD
        COMMAND cp -v shm \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)
