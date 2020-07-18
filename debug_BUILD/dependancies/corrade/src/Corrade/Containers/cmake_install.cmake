# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Corrade/Containers" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/Array.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/ArrayView.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/ArrayViewStl.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/ArrayViewStlSpan.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/constructHelpers.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/Containers.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/EnumSet.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/EnumSet.hpp"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/GrowableArray.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/LinkedList.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/Optional.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/OptionalStl.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/Pointer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/PointerStl.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/Reference.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/ScopeGuard.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/StaticArray.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/StridedArrayView.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/Tags.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Containers/ScopedExit.h"
    )
endif()

