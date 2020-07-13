cmake_minimum_required(VERSION 3.4.1)

macro(testBuilder_add_source testName sourceFile)
    set(testBuilder_SOURCES_LIST_${testName} ${testBuilder_SOURCES_LIST_${testName}};${sourceFile})
endmacro()

macro(testBuilder_add_library testName libraryName)
    set(testBuilder_LIBRARIES_LIST_${testName} ${testBuilder_LIBRARIES_LIST_${testName}};${libraryName})
endmacro()

macro(testBuilder_build testName)
    add_executable(testBuilder_executables_${testName} ${testBuilder_SOURCES_LIST_${testName}})
    target_link_libraries(testBuilder_executables_${testName} ${testBuilder_LIBRARIES_LIST_${testName}})
    add_custom_command(
            TARGET testBuilder_executables_${testName}
            POST_BUILD
            COMMAND mv testBuilder_executables_${testName} EXECUTABLES/${testName})
endmacro()
