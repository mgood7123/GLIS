cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(dependancies/WINAPI)

add_subdirectory(dependancies/freetype-2.10.2)
include_directories(dependancies/freetype-2.10.2/include)

add_subdirectory(dependancies/glm)
include_directories(dependancies/glm)

include_directories(dependancies)
include_directories(include)

set(cwd ${CMAKE_CURRENT_SOURCE_DIR})
set(GLIS_INCLUDE_DIRECTORIES ${cwd}/include ${cwd}/dependancies PARENT_SCOPE)

add_library(GLIS SHARED
        src/glis/backup/backup.cpp
        src/glis/backup/types/framebuffer.cpp
        src/glis/backup/types/renderbuffer.cpp
        src/glis/backup/types/texture.cpp
        src/glis/backup/types/program.cpp
        src/glis/font/font.cpp
        src/glis/internal/fps.cpp
        src/glis/internal/internal.cpp
        src/glis/internal/log.cpp
        src/glis/ipc/ashmem.cpp
        src/glis/ipc/ipc.cpp
        src/glis/ipc/serializer.cpp
        src/glis/ipc/server_core.cpp
        src/glis/ipc/shm.cpp
)

target_link_libraries(
        GLIS
        log
        freetype
        glm
        EGL
        GLESv3
        WinKernel
)