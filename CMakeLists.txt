cmake_minimum_required(VERSION 3.4.1)

project(GLIS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CWD ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(dependancies/WINAPI)
include_directories(${WINAPI_INCLUDE_DIRECTORIES})

if(NOT COMMAND testBuilder_build)
    add_subdirectory(dependancies/testBuilder)
    testBuilder_set_current_working_directory_to_default_binary_directory()
endif()

add_subdirectory(dependancies/font)

set(BUILD_SHARED_LIBS ON)

add_subdirectory(dependancies/freetype-2.10.2)
include_directories(dependancies/freetype-2.10.2/include)

add_subdirectory(dependancies/glm)
include_directories(dependancies/glm)

set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory(dependancies/openal-soft)
include_directories(dependancies/openal-soft)

add_subdirectory(dependancies/corrade)

set(WITH_GL ON CACHE BOOL "" FORCE)
set(TARGET_GLES ON CACHE BOOL "" FORCE)
set(TARGET_GLES2 OFF CACHE BOOL "" FORCE)
set(WITH_EGLCONTEXT ON CACHE BOOL "" FORCE)
set(WITH_MAGNUMFONT ON CACHE BOOL "" FORCE)
add_subdirectory(dependancies/magnum)

set(IMGUI_DIR ${CWD}/dependancies/imgui)
set(WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(dependancies/magnum-integration)

set(WITH_UI ON CACHE BOOL "" FORCE)
add_subdirectory(dependancies/magnum-extras)

set(WITH_STBTRUETYPEFONT ON CACHE BOOL "" FORCE)
set(WITH_FREETYPEFONT ON CACHE BOOL "" FORCE)
set(WITH_FREETYPEFONT ON CACHE BOOL "" FORCE)
set(FREETYPE_LIBRARY ${CWD}/dependancies/freetype-2.10.2)
set(FREETYPE_INCLUDE_DIRS ${CWD}/dependancies/freetype-2.10.2)
add_subdirectory(dependancies/magnum-plugins)

include_directories(dependancies)
include_directories(include)

set(GLIS_INCLUDE_DIRECTORIES ${CWD}/include ${CWD}/dependancies PARENT_SCOPE)

# corrade::rc cannot be found, unknow why this happens in android build, disable for now
#corrade_add_resource(GLIS_RESOURCES /home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/resource.conf)
#testBuilder_add_source(GLIS ${GLIS_RESOURCES})
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

testBuilder_add_library(GLIS Magnum::DebugTools)
testBuilder_add_library(GLIS Magnum::EglContext)
testBuilder_add_library(GLIS Magnum::GL)
testBuilder_add_library(GLIS Magnum::Magnum)
testBuilder_add_dependency(GLIS Magnum::MagnumFont)
testBuilder_add_library(GLIS Magnum::MeshTools)
testBuilder_add_library(GLIS Magnum::Primitives)
testBuilder_add_library(GLIS Magnum::SceneGraph)
testBuilder_add_library(GLIS Magnum::Shaders)
testBuilder_add_library(GLIS Magnum::Text)
testBuilder_add_library(GLIS Magnum::TextureTools)
testBuilder_add_library(GLIS Magnum::Trade)
testBuilder_add_library(GLIS MagnumExtras::Ui)
testBuilder_add_library(GLIS MagnumIntegration::ImGui)
testBuilder_add_dependency(GLIS MagnumPlugins::StbTrueTypeFont)
testBuilder_add_dependency(GLIS MagnumPlugins::FreeTypeFont)
testBuilder_add_library(GLIS freetype)
testBuilder_add_library(GLIS glm)
testBuilder_add_library(GLIS EGL)
testBuilder_add_library(GLIS ${WINAPI_LIBRARY})
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

# compositor
build_program(examples compositor)

# basic window creation
build_program(examples X11)
build_program(examples wayland)

# usages
build_program(examples X11_2)
build_program(examples X11_3)
build_program(examples X11_font_0)
build_program(examples X11_font_1)

# magnum
build_program(examples X11_magnum)
build_program(examples X11Cube)
build_program(examples X11_DNC_GRID)
build_program(examples wrap)

# compositor clients
build_program(compositor_clients MovingWindows)
build_program(compositor_clients MovingWindowsB)
build_program(compositor_clients compositor_start_drawing)
build_program(compositor_clients compositor_stop_drawing)
build_program(compositor_clients DefaultFramebuffer)
build_program(compositor_clients OffScreenFramebuffer)
build_program(compositor_clients Texture)
build_program(compositor_clients MYPRIVATEAPP)
build_program(compositor_clients shm)
#magnum
build_program(compositor_clients RotatingCube)