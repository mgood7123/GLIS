cmake_minimum_required(VERSION 3.4.1)

project(GLIS)

add_subdirectory(dependancies/WINAPI)

set(BUILD_SHARED_LIBS ON)

add_subdirectory(dependancies/freetype-2.10.2)
include_directories(dependancies/freetype-2.10.2/include)

add_subdirectory(dependancies/glm)
include_directories(dependancies/glm)

include_directories(dependancies)
include_directories(include)

set(cwd ${CMAKE_CURRENT_SOURCE_DIR})
set(GLIS_INCLUDE_DIRECTORIES ${cwd}/include ${cwd}/dependancies PARENT_SCOPE)

add_subdirectory(testBuilder)

testBuilder_add_source(GLIS src/glis/backup/backup.cpp)
testBuilder_add_source(GLIS src/glis/backup/types/framebuffer.cpp)
testBuilder_add_source(GLIS src/glis/backup/types/renderbuffer.cpp)
testBuilder_add_source(GLIS src/glis/backup/types/texture.cpp)
testBuilder_add_source(GLIS src/glis/backup/types/program.cpp)
testBuilder_add_source(GLIS src/glis/font/font.cpp)
testBuilder_add_source(GLIS src/glis/internal/fps.cpp)
testBuilder_add_source(GLIS src/glis/internal/internal.cpp)
testBuilder_add_source(GLIS src/glis/internal/log.cpp)
testBuilder_add_source(GLIS src/glis/ipc/ashmem.cpp)
testBuilder_add_source(GLIS src/glis/ipc/ipc.cpp)
testBuilder_add_source(GLIS src/glis/ipc/serializer.cpp)
testBuilder_add_source(GLIS src/glis/ipc/server_core.cpp)
testBuilder_add_source(GLIS src/glis/ipc/shm.cpp)

if (ANDROID)
    testBuilder_add_library(GLIS GLESv3)
    testBuilder_add_library(GLIS log)
elseif(UNIX)
    testBuilder_add_source(GLIS src/glis/internal/xdg-shell-protocol.c)
    testBuilder_add_library(GLIS GLESv2)
    testBuilder_add_library(GLIS pthread)
    testBuilder_add_library(GLIS X11)
    testBuilder_add_library(GLIS wayland-client)
    testBuilder_add_library(GLIS wayland-egl)
endif()

testBuilder_add_library(GLIS freetype)
testBuilder_add_library(GLIS glm)
testBuilder_add_library(GLIS EGL)
testBuilder_add_library(GLIS WinKernel)
testBuilder_build_shared_library(GLIS)

testBuilder_add_library(compositor GLIS)
testBuilder_add_source(compositor examples/compositor.cpp)
testBuilder_build(compositor)

testBuilder_add_library(MovingWindows GLIS)
testBuilder_add_source(MovingWindows compositor_clients/MovingWindows.cpp)
testBuilder_build(MovingWindows)

testBuilder_add_library(MovingWindowsB GLIS)
testBuilder_add_source(MovingWindowsB compositor_clients/MovingWindowsB.cpp)
testBuilder_build(MovingWindowsB)

testBuilder_add_library(compositor_start_drawing GLIS)
testBuilder_add_source(compositor_start_drawing compositor_clients/compositor_start_drawing.cpp)
testBuilder_build(compositor_start_drawing)

testBuilder_add_library(compositor_stop_drawing GLIS)
testBuilder_add_source(compositor_stop_drawing compositor_clients/compositor_stop_drawing.cpp)
testBuilder_build(compositor_stop_drawing)

testBuilder_add_library(DefaultFramebuffer GLIS)
testBuilder_add_source(DefaultFramebuffer compositor_clients/DefaultFramebuffer.cpp)
testBuilder_build(DefaultFramebuffer)

testBuilder_add_library(MYPRIVATEAPP GLIS)
testBuilder_add_source(MYPRIVATEAPP compositor_clients/MYPRIVATEAPP.cpp)
testBuilder_build(MYPRIVATEAPP)

testBuilder_add_library(shm GLIS)
testBuilder_add_source(shm compositor_clients/shm.cpp)
testBuilder_build(shm)
