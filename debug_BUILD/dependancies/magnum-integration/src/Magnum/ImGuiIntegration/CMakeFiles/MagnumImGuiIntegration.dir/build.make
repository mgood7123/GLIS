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
include dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/depend.make

# Include the progress variables for this target.
include dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/progress.make

# Include the compile flags for this target's objects.
include dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/flags.make

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.o: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/flags.make
dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.o: ../dependancies/magnum-integration/src/Magnum/ImGuiIntegration/Context.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-integration/src/Magnum/ImGuiIntegration/Context.cpp

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-integration/src/Magnum/ImGuiIntegration/Context.cpp > CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.i

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-integration/src/Magnum/ImGuiIntegration/Context.cpp -o CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.s

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.o: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/flags.make
dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.o: ../dependancies/imgui/imgui.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++  $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui.cpp

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui.cpp > CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.i

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui.cpp -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.s

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.o: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/flags.make
dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.o: ../dependancies/imgui/imgui_widgets.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++  $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_widgets.cpp

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_widgets.cpp > CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.i

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_widgets.cpp -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.s

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.o: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/flags.make
dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.o: ../dependancies/imgui/imgui_draw.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++  $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_draw.cpp

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_draw.cpp > CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.i

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_draw.cpp -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.s

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.o: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/flags.make
dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.o: ../dependancies/imgui/imgui_demo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.o"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++  $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.o -c /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_demo.cpp

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.i"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -E /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_demo.cpp > CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.i

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.s"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && /usr/bin/c++ $(CXX_DEFINES) -DIMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\" $(CXX_INCLUDES) $(CXX_FLAGS)  -Wno-old-style-cast -Wno-zero-as-null-pointer-constant -Wno-double-promotion -S /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/imgui/imgui_demo.cpp -o CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.s

# Object files for target MagnumImGuiIntegration
MagnumImGuiIntegration_OBJECTS = \
"CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.o" \
"CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.o" \
"CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.o" \
"CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.o" \
"CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.o"

# External object files for target MagnumImGuiIntegration
MagnumImGuiIntegration_EXTERNAL_OBJECTS =

Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/Context.cpp.o
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui.cpp.o
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_widgets.cpp.o
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_draw.cpp.o
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/__/__/__/__/imgui/imgui_demo.cpp.o
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/build.make
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: Debug/lib/libMagnumShaders-d.so.2.4
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: Debug/lib/libMagnumGL-d.so.2.4
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: /usr/lib/x86_64-linux-gnu/libGLESv2.so
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: Debug/lib/libMagnum-d.so.2.4
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: Debug/lib/libCorradeUtility-d.so.2.4
Debug/lib/libMagnumImGuiIntegration-d.so.2.4: dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library ../../../../../Debug/lib/libMagnumImGuiIntegration-d.so"
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/MagnumImGuiIntegration.dir/link.txt --verbose=$(VERBOSE)
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && $(CMAKE_COMMAND) -E cmake_symlink_library ../../../../../Debug/lib/libMagnumImGuiIntegration-d.so.2.4 ../../../../../Debug/lib/libMagnumImGuiIntegration-d.so.2 ../../../../../Debug/lib/libMagnumImGuiIntegration-d.so

Debug/lib/libMagnumImGuiIntegration-d.so.2: Debug/lib/libMagnumImGuiIntegration-d.so.2.4
	@$(CMAKE_COMMAND) -E touch_nocreate Debug/lib/libMagnumImGuiIntegration-d.so.2

Debug/lib/libMagnumImGuiIntegration-d.so: Debug/lib/libMagnumImGuiIntegration-d.so.2.4
	@$(CMAKE_COMMAND) -E touch_nocreate Debug/lib/libMagnumImGuiIntegration-d.so

# Rule to build all files generated by this target.
dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/build: Debug/lib/libMagnumImGuiIntegration-d.so

.PHONY : dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/build

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/clean:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration && $(CMAKE_COMMAND) -P CMakeFiles/MagnumImGuiIntegration.dir/cmake_clean.cmake
.PHONY : dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/clean

dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/depend:
	cd /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum-integration/src/Magnum/ImGuiIntegration /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : dependancies/magnum-integration/src/Magnum/ImGuiIntegration/CMakeFiles/MagnumImGuiIntegration.dir/depend
