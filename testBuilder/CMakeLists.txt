cmake_minimum_required(VERSION 3.4.1)

macro(testBuilder_add_source testName sourceFile)
    set(testBuilder_SOURCES_LIST_${testName} ${testBuilder_SOURCES_LIST_${testName}};${sourceFile})
endmacro()


# libraries can be specified as a space seperated list, or as individual calls
macro(testBuilder_add_library testName libraryName)
    set(testBuilder_LIBRARIES_LIST_${testName} ${testBuilder_LIBRARIES_LIST_${testName}};${libraryName})
endmacro()

macro(testBuilder_build testName outDirectory)
    add_executable(testBuilder_executables_${testName} ${testBuilder_SOURCES_LIST_${testName}})
    target_link_libraries(testBuilder_executables_${testName} ${testBuilder_LIBRARIES_LIST_${testName}})
    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_custom_command(
                TARGET testBuilder_executables_${testName}
                POST_BUILD
                COMMAND cp -v ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testBuilder_executables_${testName} ${outDirectory}/${testName})
    else()
        add_custom_command(
                TARGET testBuilder_executables_${testName}
                POST_BUILD
                COMMAND cp -v testBuilder_executables_${testName} ${outDirectory}/${testName})
    endif()
endmacro()

macro(testBuilder_build_shared_library testName)
    add_library(${testName} SHARED ${testBuilder_SOURCES_LIST_${testName}})
    target_link_libraries(${testName} ${testBuilder_LIBRARIES_LIST_${testName}})
endmacro()
