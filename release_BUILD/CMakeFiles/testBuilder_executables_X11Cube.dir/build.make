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
CMAKE_BINARY_DIR = /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD

# Include any dependencies generated for this target.
include CMakeFiles/testBuilder_executables_X11Cube.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testBuilder_executables_X11Cube.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testBuilder_executables_X11Cube.dir/flags.make

CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.o: CMakeFiles/testBuilder_executables_X11Cube.dir/flags.make
CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.o: ../examples/X11Cube.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/examples/X11Cube.cpp

CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/examples/X11Cube.cpp > CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.i

CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/examples/X11Cube.cpp -o CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.s

# Object files for target testBuilder_executables_X11Cube
testBuilder_executables_X11Cube_OBJECTS = \
"CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.o"

# External object files for target testBuilder_executables_X11Cube
testBuilder_executables_X11Cube_EXTERNAL_OBJECTS =

Release/bin/testBuilder_executables_X11Cube: CMakeFiles/testBuilder_executables_X11Cube.dir/examples/X11Cube.cpp.o
Release/bin/testBuilder_executables_X11Cube: CMakeFiles/testBuilder_executables_X11Cube.dir/build.make
Release/bin/testBuilder_executables_X11Cube: Release/lib/libGLIS.so
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumDebugTools.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libCorradeTestSuite.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumEglContext.a
Release/bin/testBuilder_executables_X11Cube: /usr/lib/x86_64-linux-gnu/libEGL.so
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumPrimitives.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumMeshTools.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumSceneGraph.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumShaders.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumTrade.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumUi.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumText.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumTextureTools.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnumGL.so.2.4
Release/bin/testBuilder_executables_X11Cube: /usr/lib/x86_64-linux-gnu/libGLESv2.so
Release/bin/testBuilder_executables_X11Cube: Release/lib/libMagnum.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libCorradePluginManager.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libCorradeInterconnect.so.2.4
Release/bin/testBuilder_executables_X11Cube: Release/lib/libCorradeUtility.so.2.4
Release/bin/testBuilder_executables_X11Cube: dependancies/freetype-2.10.2/libfreetype.so.6.17.2
Release/bin/testBuilder_executables_X11Cube: dependancies/WINAPI/SDK/src/Windows/Kernel/libWinKernel.a
Release/bin/testBuilder_executables_X11Cube: CMakeFiles/testBuilder_executables_X11Cube.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Release/bin/testBuilder_executables_X11Cube"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testBuilder_executables_X11Cube.dir/link.txt --verbose=$(VERBOSE)
	cp -v /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD/Release/bin/testBuilder_executables_X11Cube EXECUTABLES/X11Cube

# Rule to build all files generated by this target.
CMakeFiles/testBuilder_executables_X11Cube.dir/build: Release/bin/testBuilder_executables_X11Cube

.PHONY : CMakeFiles/testBuilder_executables_X11Cube.dir/build

CMakeFiles/testBuilder_executables_X11Cube.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testBuilder_executables_X11Cube.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testBuilder_executables_X11Cube.dir/clean

CMakeFiles/testBuilder_executables_X11Cube.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/release_BUILD/CMakeFiles/testBuilder_executables_X11Cube.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testBuilder_executables_X11Cube.dir/depend

