# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text

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
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/libMagnumText-d.so.2.4"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/libMagnumText-d.so.2"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/libMagnumText-d.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so"
         OLD_RPATH "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumText-d.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Magnum/Text" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/AbstractFont.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/AbstractFontConverter.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/AbstractGlyphCache.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/Alignment.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/Text.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/visibility.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/DistanceFieldGlyphCache.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/GlyphCache.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Text/Renderer.h"
    )
endif()

