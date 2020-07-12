cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(GLIS)
include_directories(${GLIS_INCLUDE_DIRECTORIES})

add_library(nativeegl SHARED compositor.cpp)
target_link_libraries(nativeegl android GLIS)

add_executable(MovingWindows compositor_examples/MovingWindows.cpp)
target_link_libraries(MovingWindows GLIS)
add_custom_command(
        TARGET MovingWindows
        POST_BUILD
        COMMAND cp -v MovingWindows \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(MovingWindowsB compositor_examples/MovingWindowsB.cpp)
target_link_libraries(MovingWindowsB GLIS)
add_custom_command(
        TARGET MovingWindowsB
        POST_BUILD
        COMMAND cp -v MovingWindowsB \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(compositor_start_drawing compositor_examples/compositor_start_drawing.cpp)
target_link_libraries(compositor_start_drawing GLIS)
add_custom_command(
        TARGET compositor_start_drawing
        POST_BUILD
        COMMAND cp -v compositor_start_drawing \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(compositor_stop_drawing compositor_examples/compositor_stop_drawing.cpp)
target_link_libraries(compositor_stop_drawing GLIS)
add_custom_command(
        TARGET compositor_stop_drawing
        POST_BUILD
        COMMAND cp -v compositor_stop_drawing \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(MYPRIVATEAPP compositor_examples/MYPRIVATEAPP.cpp)
target_link_libraries(MYPRIVATEAPP GLIS)
add_custom_command(
        TARGET MYPRIVATEAPP
        POST_BUILD
        COMMAND cp -v MYPRIVATEAPP \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(DefaultFramebuffer compositor_examples/DefaultFramebuffer.cpp)
target_link_libraries(DefaultFramebuffer GLIS)
add_custom_command(
        TARGET DefaultFramebuffer
        POST_BUILD
        COMMAND cp -v DefaultFramebuffer \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)

add_executable(shm_demo compositor_examples/shm.cpp)
target_link_libraries(shm_demo GLIS)
add_custom_command(
        TARGET shm_demo
        POST_BUILD
        COMMAND cp -v shm_demo \"${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI}\"
)
