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
include dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/depend.make

# Include the progress variables for this target.
include dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/flags.make

dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.o: dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/flags.make
dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.o: ../dependancies/magnum/src/dummy.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/dummy.cpp

dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/dummy.cpp > CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.i

dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/dummy.cpp -o CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.s

# Object files for target MagnumEglContext
MagnumEglContext_OBJECTS = \
"CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.o"

# External object files for target MagnumEglContext
MagnumEglContext_EXTERNAL_OBJECTS = \
"/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContextObjects.dir/Implementation/OpenGLFunctionLoader.cpp.o" \
"/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContextObjects.dir/__/__/MagnumExternal/OpenGL/GLES3/flextGLPlatform.cpp.o"

Debug/lib/libMagnumEglContext-d.a: dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/__/__/dummy.cpp.o
Debug/lib/libMagnumEglContext-d.a: dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContextObjects.dir/Implementation/OpenGLFunctionLoader.cpp.o
Debug/lib/libMagnumEglContext-d.a: dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContextObjects.dir/__/__/MagnumExternal/OpenGL/GLES3/flextGLPlatform.cpp.o
Debug/lib/libMagnumEglContext-d.a: dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/build.make
Debug/lib/libMagnumEglContext-d.a: dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../../../Debug/lib/libMagnumEglContext-d.a"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform && $(CMAKE_COMMAND) -P CMakeFiles/MagnumEglContext.dir/cmake_clean_target.cmake
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MagnumEglContext.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/build: Debug/lib/libMagnumEglContext-d.a

.PHONY : dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/build

dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform && $(CMAKE_COMMAND) -P CMakeFiles/MagnumEglContext.dir/cmake_clean.cmake
.PHONY : dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/clean

dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/magnum/src/Magnum/Platform/CMakeFiles/MagnumEglContext.dir/depend
