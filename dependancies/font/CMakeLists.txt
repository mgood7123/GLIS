cmake_minimum_required(VERSION 3.0)

project(font_structure)

if(NOT COMMAND testBuilder_build)
    add_subdirectory(testBuilder)
    testBuilder_set_current_working_directory_to_default_binary_directory()
endif()

if(NOT AnyOpt_INCLUDE)
    add_subdirectory(AnyOpt)
    include_directories(${AnyOpt_INCLUDE})
endif()

if(NOT WINAPI_LIBRARY)
    add_subdirectory(WINAPI)
    include_directories(${WINAPI_INCLUDE})
endif()

testBuilder_add_source(f main.cpp)
testBuilder_add_library(f ${WINAPI_LIBRARY})
testBuilder_build(f EXECUTABLES)