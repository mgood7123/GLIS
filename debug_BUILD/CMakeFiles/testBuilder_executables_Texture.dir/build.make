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
include CMakeFiles/testBuilder_executables_Texture.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testBuilder_executables_Texture.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testBuilder_executables_Texture.dir/flags.make

CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.o: CMakeFiles/testBuilder_executables_Texture.dir/flags.make
CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.o: ../compositor_clients/Texture.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/compositor_clients/Texture.cpp

CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/compositor_clients/Texture.cpp > CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.i

CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/compositor_clients/Texture.cpp -o CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.s

# Object files for target testBuilder_executables_Texture
testBuilder_executables_Texture_OBJECTS = \
"CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.o"

# External object files for target testBuilder_executables_Texture
testBuilder_executables_Texture_EXTERNAL_OBJECTS =

Debug/bin/testBuilder_executables_Texture: CMakeFiles/testBuilder_executables_Texture.dir/compositor_clients/Texture.cpp.o
Debug/bin/testBuilder_executables_Texture: CMakeFiles/testBuilder_executables_Texture.dir/build.make
Debug/bin/testBuilder_executables_Texture: Debug/lib/libGLIS.so
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumDebugTools-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libCorradeTestSuite-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumEglContext-d.a
Debug/bin/testBuilder_executables_Texture: /usr/lib/x86_64-linux-gnu/libEGL.so
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumPrimitives-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumMeshTools-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumSceneGraph-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumShaders-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumTrade-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumUi-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumText-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumTextureTools-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnumGL-d.so.2.4
Debug/bin/testBuilder_executables_Texture: /usr/lib/x86_64-linux-gnu/libGLESv2.so
Debug/bin/testBuilder_executables_Texture: Debug/lib/libMagnum-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libCorradePluginManager-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libCorradeInterconnect-d.so.2.4
Debug/bin/testBuilder_executables_Texture: Debug/lib/libCorradeUtility-d.so.2.4
Debug/bin/testBuilder_executables_Texture: dependancies/freetype-2.10.2/libfreetyped.so.6.17.2
Debug/bin/testBuilder_executables_Texture: dependancies/WINAPI/SDK/src/Windows/Kernel/libWinKernel.a
Debug/bin/testBuilder_executables_Texture: CMakeFiles/testBuilder_executables_Texture.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Debug/bin/testBuilder_executables_Texture"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testBuilder_executables_Texture.dir/link.txt --verbose=$(VERBOSE)
	cp -v /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/bin/testBuilder_executables_Texture EXECUTABLES/Texture

# Rule to build all files generated by this target.
CMakeFiles/testBuilder_executables_Texture.dir/build: Debug/bin/testBuilder_executables_Texture

.PHONY : CMakeFiles/testBuilder_executables_Texture.dir/build

CMakeFiles/testBuilder_executables_Texture.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testBuilder_executables_Texture.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testBuilder_executables_Texture.dir/clean

CMakeFiles/testBuilder_executables_Texture.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles/testBuilder_executables_Texture.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testBuilder_executables_Texture.dir/depend

