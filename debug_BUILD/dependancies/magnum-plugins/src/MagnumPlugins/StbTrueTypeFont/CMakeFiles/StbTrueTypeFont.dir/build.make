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
include dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/depend.make

# Include the progress variables for this target.
include dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/flags.make

dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.o: dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/flags.make
dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.o: ../dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/StbTrueTypeFont.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/StbTrueTypeFont.cpp

dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/StbTrueTypeFont.cpp > CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.i

dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/StbTrueTypeFont.cpp -o CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.s

# Object files for target StbTrueTypeFont
StbTrueTypeFont_OBJECTS = \
"CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.o"

# External object files for target StbTrueTypeFont
StbTrueTypeFont_EXTERNAL_OBJECTS =

Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/StbTrueTypeFont.cpp.o
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/build.make
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: Debug/lib/libMagnumText-d.so.2.4
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: Debug/lib/libMagnumTextureTools-d.so.2.4
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: Debug/lib/libCorradePluginManager-d.so.2.4
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: Debug/lib/libMagnumGL-d.so.2.4
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: Debug/lib/libMagnum-d.so.2.4
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: Debug/lib/libCorradeUtility-d.so.2.4
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: /usr/lib/x86_64-linux-gnu/libGLESv2.so
Debug/lib/magnum-d/fonts/StbTrueTypeFont.so: dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module ../../../../../Debug/lib/magnum-d/fonts/StbTrueTypeFont.so"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/StbTrueTypeFont.dir/link.txt --verbose=$(VERBOSE)
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont && /usr/bin/cmake -E copy /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/StbTrueTypeFont.conf /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/magnum-d/fonts/StbTrueTypeFont.conf

# Rule to build all files generated by this target.
dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/build: Debug/lib/magnum-d/fonts/StbTrueTypeFont.so

.PHONY : dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/build

dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont && $(CMAKE_COMMAND) -P CMakeFiles/StbTrueTypeFont.dir/cmake_clean.cmake
.PHONY : dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/clean

dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/magnum-plugins/src/MagnumPlugins/StbTrueTypeFont/CMakeFiles/StbTrueTypeFont.dir/depend

