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
CMAKE_BINARY_DIR = /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD

# Include any dependencies generated for this target.
include dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/depend.make

# Include the progress variables for this target.
include dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/flags.make

dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.o: dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/flags.make
dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.o: ../dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/FreeTypeFont.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/FreeTypeFont.cpp

dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/FreeTypeFont.cpp > CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.i

dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/FreeTypeFont.cpp -o CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.s

# Object files for target FreeTypeFont
FreeTypeFont_OBJECTS = \
"CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.o"

# External object files for target FreeTypeFont
FreeTypeFont_EXTERNAL_OBJECTS =

Debug/lib/magnum-d/fonts/FreeTypeFont.so: dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/FreeTypeFont.cpp.o
Debug/lib/magnum-d/fonts/FreeTypeFont.so: dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/build.make
Debug/lib/magnum-d/fonts/FreeTypeFont.so: Debug/lib/libMagnumText-d.so.2.4
Debug/lib/magnum-d/fonts/FreeTypeFont.so: Debug/lib/libMagnumTextureTools-d.so.2.4
Debug/lib/magnum-d/fonts/FreeTypeFont.so: Debug/lib/libCorradePluginManager-d.so.2.4
Debug/lib/magnum-d/fonts/FreeTypeFont.so: Debug/lib/libMagnumGL-d.so.2.4
Debug/lib/magnum-d/fonts/FreeTypeFont.so: Debug/lib/libMagnum-d.so.2.4
Debug/lib/magnum-d/fonts/FreeTypeFont.so: Debug/lib/libCorradeUtility-d.so.2.4
Debug/lib/magnum-d/fonts/FreeTypeFont.so: /usr/lib/x86_64-linux-gnu/libGLESv2.so
Debug/lib/magnum-d/fonts/FreeTypeFont.so: dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module ../../../../../Debug/lib/magnum-d/fonts/FreeTypeFont.so"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FreeTypeFont.dir/link.txt --verbose=$(VERBOSE)
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont && /usr/bin/cmake -E copy /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/FreeTypeFont.conf /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/magnum-d/fonts/FreeTypeFont.conf

# Rule to build all files generated by this target.
dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/build: Debug/lib/magnum-d/fonts/FreeTypeFont.so

.PHONY : dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/build

dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont && $(CMAKE_COMMAND) -P CMakeFiles/FreeTypeFont.dir/cmake_clean.cmake
.PHONY : dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/clean

dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/magnum-plugins/src/MagnumPlugins/FreeTypeFont/CMakeFiles/FreeTypeFont.dir/depend

