# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD

# Include any dependencies generated for this target.
include dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/depend.make

# Include the progress variables for this target.
include dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/flags.make

dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.o: dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/flags.make
dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.o: ../dependancies/AnyOpt/tests/AnyOpt_Tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/AnyOpt/tests/AnyOpt_Tests.cpp

dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/AnyOpt/tests/AnyOpt_Tests.cpp > CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.i

dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/AnyOpt/tests/AnyOpt_Tests.cpp -o CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.s

# Object files for target testBuilder_executables_AnyOpt_Tests
testBuilder_executables_AnyOpt_Tests_OBJECTS = \
"CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.o"

# External object files for target testBuilder_executables_AnyOpt_Tests
testBuilder_executables_AnyOpt_Tests_EXTERNAL_OBJECTS =

Debug/bin/testBuilder_executables_AnyOpt_Tests: dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/AnyOpt_Tests.cpp.o
Debug/bin/testBuilder_executables_AnyOpt_Tests: dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/build.make
Debug/bin/testBuilder_executables_AnyOpt_Tests: lib/libgtest_maind.a
Debug/bin/testBuilder_executables_AnyOpt_Tests: lib/libgtestd.a
Debug/bin/testBuilder_executables_AnyOpt_Tests: dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../Debug/bin/testBuilder_executables_AnyOpt_Tests"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/link.txt --verbose=$(VERBOSE)
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests && cp -v /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/bin/testBuilder_executables_AnyOpt_Tests /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/EXECUTABLES/AnyOpt_Tests

# Rule to build all files generated by this target.
dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/build: Debug/bin/testBuilder_executables_AnyOpt_Tests

.PHONY : dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/build

dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests && $(CMAKE_COMMAND) -P CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/cmake_clean.cmake
.PHONY : dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/clean

dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/AnyOpt/tests /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/AnyOpt/tests/CMakeFiles/testBuilder_executables_AnyOpt_Tests.dir/depend

