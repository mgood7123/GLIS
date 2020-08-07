# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility

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
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libCorradeUtility-d.so.2.4"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libCorradeUtility-d.so.2"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libCorradeUtility-d.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libCorradeUtility-d.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Corrade/Utility" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Algorithms.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Arguments.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/AbstractHash.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Assert.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Configuration.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/ConfigurationGroup.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/ConfigurationValue.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Debug.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/DebugStl.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Directory.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Endianness.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/EndiannessBatch.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Format.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/FormatStl.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Macros.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/MurmurHash2.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Resource.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Sha1.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/String.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/StlForwardArray.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/StlForwardString.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/StlForwardTuple.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/StlForwardVector.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/StlMath.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/System.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/TypeTraits.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Unicode.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/utilities.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Utility.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/VisibilityMacros.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/visibility.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/FileWatcher.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/Tweakable.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/corrade/src/Corrade/Utility/TweakableParser.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/corrade-rc" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/corrade-rc")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/corrade-rc"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/bin/corrade-rc")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/corrade-rc" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/corrade-rc")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/corrade-rc")
    endif()
  endif()
endif()

