cmake_minimum_required(VERSION 3.4.1)

project(GLIS)

add_subdirectory(dependancies/WINAPI)

set(BUILD_SHARED_LIBS ON)

add_subdirectory(dependancies/freetype-2.10.2)
include_directories(dependancies/freetype-2.10.2/include)

add_subdirectory(dependancies/glm)
include_directories(dependancies/glm)

add_subdirectory(dependancies/corrade) # build with everything
#add_subdirectory(dependancies/corrade EXCLUDE_FROM_ALL) # so only things you use are built

# https://doc.magnum.graphics/magnum/requires-gl.html

set(WITH_GL ON CACHE BOOL "" FORCE)
set(TARGET_GLES ON CACHE BOOL "" FORCE)
set(WITH_EGLCONTEXT ON CACHE BOOL "" FORCE)
#set(TARGET_DESKTOP_GLES ON CACHE BOOL "" FORCE)
#set(TARGET_VK ON CACHE BOOL "" FORCE)
#set(WITH_MAGNUMFONT ON CACHE BOOL "" FORCE)
#set(WITH_MAGNUMFONTCONVERTER ON CACHE BOOL "" FORCE)
add_subdirectory(dependancies/magnum) # build with everything
#add_subdirectory(dependancies/magnum EXCLUDE_FROM_ALL)  # so only things you use are built

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
testBuilder_add_source(GLIS src/glis/compositor/compositor.cpp)
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
    testBuilder_add_library(GLIS android)
    testBuilder_add_library(GLIS log)
elseif(UNIX)
    testBuilder_add_source(GLIS src/glis/internal/xdg-shell-protocol.c)
    testBuilder_add_library(GLIS GLESv2)
    testBuilder_add_library(GLIS pthread)
    testBuilder_add_library(GLIS X11)
    testBuilder_add_library(GLIS wayland-client)
    testBuilder_add_library(GLIS wayland-egl)
endif()

testBuilder_add_library(GLIS Magnum::Magnum)
testBuilder_add_library(GLIS Magnum::GL)
testBuilder_add_library(GLIS Magnum::EglContext)
testBuilder_add_library(GLIS freetype)
testBuilder_add_library(GLIS glm)
testBuilder_add_library(GLIS EGL)
testBuilder_add_library(GLIS WinKernel)
testBuilder_build_shared_library(GLIS)

# BUILD PROGRAMS

macro(build_program directory name)
    testBuilder_add_library(${name} GLIS)
    testBuilder_add_source(${name} ${directory}/${name}.cpp)
    if(ANDROID)
        testBuilder_build(${name} ${CMAKE_SOURCE_DIR}/executables/Arch/${CMAKE_ANDROID_ARCH_ABI})
    elseif(UNIX)
        testBuilder_build(${name} EXECUTABLES)
    endif()
endmacro()

build_program(examples compositor)
build_program(examples X11)
build_program(examples X11_2)
build_program(examples X11_3)
build_program(examples wayland)
build_program(examples X11_magnum)

build_program(compositor_clients MovingWindows)
build_program(compositor_clients MovingWindowsB)
build_program(compositor_clients compositor_start_drawing)
build_program(compositor_clients compositor_stop_drawing)
build_program(compositor_clients DefaultFramebuffer)
build_program(compositor_clients OffScreenFramebuffer)
build_program(compositor_clients Texture)
build_program(compositor_clients MYPRIVATEAPP)
build_program(compositor_clients shm)