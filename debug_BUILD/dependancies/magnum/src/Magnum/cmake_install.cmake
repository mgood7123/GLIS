# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum

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
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libMagnum-d.so.2.4"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libMagnum-d.so.2"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib/libMagnum-d.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so"
         OLD_RPATH "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/Debug/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnum-d.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Magnum" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/AbstractResourceLoader.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Array.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/British.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/DimensionTraits.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/FileCallback.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Image.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/ImageView.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Magnum.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Mesh.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/PixelFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/PixelStorage.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Resource.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/ResourceManager.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Sampler.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Tags.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Timeline.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/Types.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/VertexFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/visibility.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Magnum" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/configure.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/version.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Animation/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Math/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Platform/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/DebugTools/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/GL/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/MeshTools/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Primitives/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/SceneGraph/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Shaders/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Text/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/TextureTools/cmake_install.cmake")
  include("/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_BUILD/dependancies/magnum/src/Magnum/Trade/cmake_install.cmake")

endif()

