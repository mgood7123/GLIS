# Install script for directory: /home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL

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
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so.2"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/libMagnumGL-d.so.2.4"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/libMagnumGL-d.so.2"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so.2.4"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so.2"
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
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib/libMagnumGL-d.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so"
         OLD_RPATH "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/debug_asan_BUILD/Debug/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libMagnumGL-d.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/./include/Magnum/GL" TYPE FILE FILES
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/AbstractFramebuffer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/AbstractObject.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/AbstractQuery.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/AbstractShaderProgram.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/AbstractTexture.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Attribute.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Buffer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Context.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/CubeMapTexture.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/DefaultFramebuffer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Extensions.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Framebuffer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/GL.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Mesh.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/MeshView.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/OpenGL.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/PixelFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Renderbuffer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/RenderbufferFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Renderer.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Sampler.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Shader.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Texture.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/TextureFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/TimeQuery.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/Version.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/visibility.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/BufferImage.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/PrimitiveQuery.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/TextureArray.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/TransformFeedback.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/DebugOutput.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/BufferTexture.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/BufferTextureFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/CubeMapTextureArray.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/ImageFormat.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/MultisampleTexture.h"
    "/home/smallville7123/AndroidCompositor/app/src/main/jni/GLIS/dependancies/magnum/src/Magnum/GL/SampleQuery.h"
    )
endif()

