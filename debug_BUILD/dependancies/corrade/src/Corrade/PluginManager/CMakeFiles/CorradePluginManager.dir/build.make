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
include dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/depend.make

# Include the progress variables for this target.
include dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/flags.make

dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.o: dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/flags.make
dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.o: ../dependancies/corrade/src/Corrade/PluginManager/AbstractManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/PluginManager/AbstractManager.cpp

dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/PluginManager/AbstractManager.cpp > CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.i

dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/PluginManager/AbstractManager.cpp -o CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.s

# Object files for target CorradePluginManager
CorradePluginManager_OBJECTS = \
"CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.o"

# External object files for target CorradePluginManager
CorradePluginManager_EXTERNAL_OBJECTS = \
"/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManagerObjects.dir/AbstractPlugin.cpp.o" \
"/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManagerObjects.dir/PluginMetadata.cpp.o"

Debug/lib/libCorradePluginManager-d.so.2.4: dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/AbstractManager.cpp.o
Debug/lib/libCorradePluginManager-d.so.2.4: dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManagerObjects.dir/AbstractPlugin.cpp.o
Debug/lib/libCorradePluginManager-d.so.2.4: dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManagerObjects.dir/PluginMetadata.cpp.o
Debug/lib/libCorradePluginManager-d.so.2.4: dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/build.make
Debug/lib/libCorradePluginManager-d.so.2.4: Debug/lib/libCorradeUtility-d.so.2.4
Debug/lib/libCorradePluginManager-d.so.2.4: dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../../../../../Debug/lib/libCorradePluginManager-d.so"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CorradePluginManager.dir/link.txt --verbose=$(VERBOSE)
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager && $(CMAKE_COMMAND) -E cmake_symlink_library ../../../../../Debug/lib/libCorradePluginManager-d.so.2.4 ../../../../../Debug/lib/libCorradePluginManager-d.so.2 ../../../../../Debug/lib/libCorradePluginManager-d.so

Debug/lib/libCorradePluginManager-d.so.2: Debug/lib/libCorradePluginManager-d.so.2.4
	@$(CMAKE_COMMAND) -E touch_nocreate Debug/lib/libCorradePluginManager-d.so.2

Debug/lib/libCorradePluginManager-d.so: Debug/lib/libCorradePluginManager-d.so.2.4
	@$(CMAKE_COMMAND) -E touch_nocreate Debug/lib/libCorradePluginManager-d.so

# Rule to build all files generated by this target.
dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/build: Debug/lib/libCorradePluginManager-d.so

.PHONY : dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/build

dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager && $(CMAKE_COMMAND) -P CMakeFiles/CorradePluginManager.dir/cmake_clean.cmake
.PHONY : dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/clean

dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/PluginManager /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/corrade/src/Corrade/PluginManager/CMakeFiles/CorradePluginManager.dir/depend

