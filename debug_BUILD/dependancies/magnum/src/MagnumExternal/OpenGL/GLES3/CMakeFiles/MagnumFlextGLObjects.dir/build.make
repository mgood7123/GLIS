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
include dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/depend.make

# Include the progress variables for this target.
include dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flags.make

dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.o: dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flags.make
dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.o: ../dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/flextGL.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/flextGL.cpp

dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/flextGL.cpp > CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.i

dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/flextGL.cpp -o CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.s

MagnumFlextGLObjects: dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/flextGL.cpp.o
MagnumFlextGLObjects: dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/build.make

.PHONY : MagnumFlextGLObjects

# Rule to build all files generated by this target.
dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/build: MagnumFlextGLObjects

.PHONY : dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/build

dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3 && $(CMAKE_COMMAND) -P CMakeFiles/MagnumFlextGLObjects.dir/cmake_clean.cmake
.PHONY : dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/clean

dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3 /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3 /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/magnum/src/MagnumExternal/OpenGL/GLES3/CMakeFiles/MagnumFlextGLObjects.dir/depend

