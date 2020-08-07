cmake_minimum_required(VERSION 3.0)

project(font_structure)

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-scope")

if(NOT WINAPI_LIBRARY)
    add_subdirectory(WINAPI)
    include_directories(${WINAPI_INCLUDE})
endif()

if(NOT testBuilder_build)
    add_subdirectory(testBuilder)
endif()

testBuilder_add_source(f main.cpp)
testBuilder_add_library(f ${WINAPI_LIBRARY})
testBuilder_build(f EXECUTABLES)