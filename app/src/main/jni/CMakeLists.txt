cmake_minimum_required(VERSION 3.4.1)
add_subdirectory(WINAPI)
add_library(nativeegl SHARED compositor.cpp shm.cpp ashmem.cpp)
target_link_libraries(nativeegl android log EGL GLESv3 WinKernel)

add_executable(compositor_start_drawing compositor_examples/compositor_start_drawing.cpp shm.cpp ashmem.cpp)
target_link_libraries(compositor_start_drawing log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET compositor_start_drawing
        POST_BUILD
        COMMAND cp -v compositor_start_drawing \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(compositor_stop_drawing compositor_examples/compositor_stop_drawing.cpp shm.cpp ashmem.cpp)
target_link_libraries(compositor_stop_drawing log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET compositor_stop_drawing
        POST_BUILD
        COMMAND cp -v compositor_stop_drawing \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(MYPRIVATEAPP compositor_examples/MYPRIVATEAPP.cpp shm.cpp ashmem.cpp)
target_link_libraries(MYPRIVATEAPP log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MYPRIVATEAPP
        POST_BUILD
        COMMAND cp -v MYPRIVATEAPP \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(MovingWindows compositor_examples/MovingWindows.cpp shm.cpp ashmem.cpp)
target_link_libraries(MovingWindows log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET MovingWindows
        POST_BUILD
        COMMAND cp -v MovingWindows \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(DefaultFramebuffer compositor_examples/DefaultFramebuffer.cpp shm.cpp ashmem.cpp)
target_link_libraries(DefaultFramebuffer log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET DefaultFramebuffer
        POST_BUILD
        COMMAND cp -v DefaultFramebuffer \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(shm compositor_examples/shm.cpp shm.cpp ashmem.cpp)
target_link_libraries(shm log EGL GLESv3 WinKernel)
add_custom_command(
        TARGET shm
        POST_BUILD
        COMMAND cp -v shm \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)
