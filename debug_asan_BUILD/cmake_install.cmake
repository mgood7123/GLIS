# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/testBuilder/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/AnyOpt/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/WINAPI/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/font/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/freetype-2.10.2/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/glm/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/openal-soft/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/corrade/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-integration/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-extras/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/dependancies/magnum-plugins/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
