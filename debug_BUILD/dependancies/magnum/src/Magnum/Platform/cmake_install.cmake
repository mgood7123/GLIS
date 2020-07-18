# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Magnum/Platform" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform/GLContext.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform/Platform.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform/Screen.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform/ScreenedApplication.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Platform/ScreenedApplication.hpp"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libMagnumEglContext-d.a")
endif()

