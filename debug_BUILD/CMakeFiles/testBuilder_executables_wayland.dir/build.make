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
include CMakeFiles/testBuilder_executables_wayland.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testBuilder_executables_wayland.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testBuilder_executables_wayland.dir/flags.make

CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.o: CMakeFiles/testBuilder_executables_wayland.dir/flags.make
CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.o: ../examples/wayland.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/examples/wayland.cpp

CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/examples/wayland.cpp > CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.i

CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/examples/wayland.cpp -o CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.s

# Object files for target testBuilder_executables_wayland
testBuilder_executables_wayland_OBJECTS = \
"CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.o"

# External object files for target testBuilder_executables_wayland
testBuilder_executables_wayland_EXTERNAL_OBJECTS =

Debug/bin/testBuilder_executables_wayland: CMakeFiles/testBuilder_executables_wayland.dir/examples/wayland.cpp.o
Debug/bin/testBuilder_executables_wayland: CMakeFiles/testBuilder_executables_wayland.dir/build.make
Debug/bin/testBuilder_executables_wayland: Debug/lib/libGLIS.so
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumDebugTools-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libCorradeTestSuite-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumEglContext-d.a
Debug/bin/testBuilder_executables_wayland: /usr/lib/x86_64-linux-gnu/libEGL.so
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumPrimitives-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumMeshTools-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumSceneGraph-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumTrade-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumUi-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumText-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumTextureTools-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libCorradePluginManager-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libCorradeInterconnect-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumImGuiIntegration-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumShaders-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnumGL-d.so.2.4
Debug/bin/testBuilder_executables_wayland: /usr/lib/x86_64-linux-gnu/libGLESv2.so
Debug/bin/testBuilder_executables_wayland: Debug/lib/libMagnum-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libCorradeUtility-d.so.2.4
Debug/bin/testBuilder_executables_wayland: Debug/lib/libfreetyped.so.6.17.2
Debug/bin/testBuilder_executables_wayland: Debug/lib/libWinKernel.so
Debug/bin/testBuilder_executables_wayland: CMakeFiles/testBuilder_executables_wayland.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Debug/bin/testBuilder_executables_wayland"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testBuilder_executables_wayland.dir/link.txt --verbose=$(VERBOSE)
	cp -v /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/bin/testBuilder_executables_wayland /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/EXECUTABLES/wayland

# Rule to build all files generated by this target.
CMakeFiles/testBuilder_executables_wayland.dir/build: Debug/bin/testBuilder_executables_wayland

.PHONY : CMakeFiles/testBuilder_executables_wayland.dir/build

CMakeFiles/testBuilder_executables_wayland.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testBuilder_executables_wayland.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testBuilder_executables_wayland.dir/clean

CMakeFiles/testBuilder_executables_wayland.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles/testBuilder_executables_wayland.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testBuilder_executables_wayland.dir/depend

