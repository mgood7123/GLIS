//
// Created by konek on 7/26/2019.
//

#ifndef GLNE_GLA_H
#define GLNE_GLA_H

#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include "logger.h"

#define LOG_TAG "EglSample"

struct GLINITIALIZATIONSTRUCTURE {
    EGLDisplay display;
    const GLint *attrib_list;
    EGLConfig configs;
    EGLint num_config;
    EGLContext context;
    EGLSurface surface;
    ANativeWindow * native_window;
    AHardwareBuffer * native_buffer;
    GLint width;
    GLint height;
};

bool init_display(struct GLINITIALIZATIONSTRUCTURE & GLIS) {
    /*
Name
eglGetDisplay — return an EGL display connection

C Specification
EGLDisplay eglGetDisplay( NativeDisplayType native_display);

Parameters
native_display
Specifies the display to connect to. EGL_DEFAULT_DISPLAY indicates the default display.

Description
eglGetDisplay obtains the EGL display connection for the native display native_display.
The behavior of eglGetDisplay is similar to that of eglGetPlatformDisplay, but is specified in terms of implementation-specific behavior rather than platform-specific extensions. As for eglGetPlatformDisplay, EGL considers the returned EGLDisplay as belonging to the same platform as display_id. However, the set of platforms to which display_id is permitted to belong, as well as the actual type of display_id, are implementation-specific. If display_id is EGL_DEFAULT_DISPLAY, a default display is returned. Multiple calls made to eglGetDisplay with the same display_id will return the same EGLDisplay handle.
If display_id is EGL_DEFAULT_DISPLAY, a default display connection is returned.
If no display connection matching native_display is available, EGL_NO_DISPLAY is returned. No error is generated.
Use eglInitialize to initialize the display connection.
 */
    if ((GLIS.display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOG_ERROR("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }
    /*
Name
eglInitialize — initialize an EGL display connection

C Specification
EGLBoolean eglInitialize(
                          EGLDisplay display,
                          EGLint * major,
                          EGLint * minor);

Parameters
display
    Specifies the EGL display connection to initialize.
major
    Returns the major version number of the EGL implementation. May be NULL.
minor
    Returns the minor version number of the EGL implementation. May be NULL.

Description
eglInitialize initialized the EGL display connection obtained with eglGetDisplay. Initializing an already initialized EGL display connection has no effect besides returning the version numbers.
major and minor do not return values if they are specified as NULL.
Use eglTerminate to release resources associated with an EGL display connection.

Errors
EGL_FALSE is returned if eglInitialize fails, EGL_TRUE otherwise. major and minor are not modified when EGL_FALSE is returned.
EGL_BAD_DISPLAY is generated if display is not an EGL display connection.
EGL_NOT_INITIALIZED is generated if display cannot be initialized.
     */
    if (eglInitialize(GLIS.display, nullptr, nullptr) != EGL_TRUE) {
        LOG_ERROR("eglInitialize() returned error %d", eglGetError());
        return false;
    }
    return true;
}

bool init_config(struct GLINITIALIZATIONSTRUCTURE & GLIS) {
    /*
Name
eglChooseConfig — return a list of EGL frame buffer configurations that match specified attributes

C Specification
EGLBoolean eglChooseConfig(
                        EGLDisplay display,
                        EGLint const * attrib_list,
                        EGLConfig * configs,
                        EGLint config_size,
                        EGLint * num_config);

Parameters
display
    Specifies the EGL display connection.
attrib_list
    Specifies attributes required to match by configs.
configs
    Returns an array of frame buffer configurations.
config_size
    Specifies the size of the array of frame buffer configurations.
num_config
    Returns the number of frame buffer configurations returned.

Description
eglChooseConfig returns in configs a list of all EGL frame buffer configurations that match the attributes specified in attrib_list. The returned EGLConfigs can be used in any EGL function that requires an EGL frame buffer configuration.
If configs is not NULL, up to config_size configs will be returned in the array pointed to by configs. The number of configs actually returned will be returned in *num_config.
If configs is NULL, no configs will be returned in configs. Instead, the total number of configs matching attrib_list will be returned in *num_config. In this case config_size is ignored. This form of eglChooseConfig is used to determine the number of matching frame buffer configurations, followed by allocating an array of EGLConfig to pass into another call to eglChooseConfig with all other parameters unchanged.
All attributes in attrib_list, including boolean attributes, are immediately followed by the corresponding desired value. The list is terminated with EGL_NONE. If an attribute is not specified in attrib_list then the default value (see below) is used (and the attribute is said to be specified implicitly). For example, if EGL_DEPTH_SIZE is not specified then it is assumed to be zero. For some attributes, the default is EGL_DONT_CARE meaning that any value is OK for this attribute, so the attribute will not be checked.
Attributes are matched in an attribute-specific manner. Some of the attributes, such as EGL_LEVEL, must match the specified value exactly. Others, such as, EGL_RED_SIZE must meet or exceed the specified minimum values. If more than one EGL frame buffer configuration matching all attributes is found, then a list of configurations, sorted according to the ``best'' match criteria, is returned. The match criteria for each attribute and the exact sorting order is defined below.
For the bitmask attributes EGL_CONFORMANT, EGL_RENDERABLE_TYPE, and EGL_SURFACE_TYPE, only the nonzero bits of the mask are considered when matching. Any bits that are zero in the specified bitmask attribute value may be either zero or one in the resulting config's attribute value.

see https://www.khronos.org/registry/EGL/sdk/docs/man/html/eglChooseConfig.xhtml for list of available attributes

Examples
The following example specifies a frame buffer configuration in the normal frame buffer (not an overlay or underlay). The returned frame buffer configuration supports a color buffer with at least 4 bits each of red, green and blue, and possibly no alpha bits. The code shown in the example may or may not have a depth buffer, or a stencil buffer.
EGLint const attrib_list[] = {
    EGL_RED_SIZE, 4,
    EGL_GREEN_SIZE, 4,
    EGL_BLUE_SIZE, 4,
    EGL_NONE
};

Notes
EGL_RENDERABLE_TYPE bit EGL_OPENGL_BIT, and EGL_SURFACE_TYPE bits EGL_MULTISAMPLE_RESOLVE_BOX_BIT and EGL_SWAP_BEHAVIOR_PRESERVED_BIT are supported only if the EGL version is 1.4 or greater.
EGL_CONFORMANT, EGL_MATCH_NATIVE_PIXMAP, EGL_RENDERABLE_TYPE bit EGL_OPENGL_ES2_BIT, and EGL_SURFACE_TYPE bits EGL_VG_ALPHA_FORMAT_PRE_BIT and EGL_VG_COLORSPACE_LINEAR_BIT are supported only if the EGL version is 1.3 or greater.
EGL_ALPHA_MASK_SIZE, EGL_COLOR_BUFFER_TYPE, EGL_LUMINANCE_SIZE, EGL_RENDERABLE_TYPE, and EGL_RENDERABLE_TYPE bits EGL_OPENGL_ES_BIT and EGL_OPENVG_BIT are supported only if the EGL version is 1.2 or greater.
If OpenGL or OpenGL ES rendering is supported for a luminance color buffer, it is treated as RGB rendering with the value of GL_RED_BITS equal to EGL_LUMINANCE_SIZE and the values of GL_GREEN_BITS and GL_BLUE_BITS equal to zero. The red component of fragments is written to the luminance channel of the color buffer while the green and blue components are discarded.
eglGetConfigs and eglGetConfigAttrib can be used to implement selection algorithms other than the generic one implemented by eglChooseConfig. Call eglGetConfigs to retrieve all the frame buffer configurations, or alternatively, all the frame buffer configurations with a particular set of attributes. Next call eglGetConfigAttrib to retrieve additional attributes for the frame buffer configurations and then select between them.
EGL implementors are strongly discouraged, but not proscribed, from changing the selection algorithm used by eglChooseConfig. Therefore, selections may change from release to release of the client-side library.

Errors
EGL_FALSE is returned on failure, EGL_TRUE otherwise. configs and num_config are not modified when EGL_FALSE is returned.
EGL_BAD_DISPLAY is generated if display is not an EGL display connection.
EGL_BAD_ATTRIBUTE is generated if attribute_list contains an invalid frame buffer configuration attribute or an attribute value that is unrecognized or out of range.
EGL_NOT_INITIALIZED is generated if display has not been initialized.
EGL_BAD_PARAMETER is generated if num_config is NULL.
 */
    if (!eglChooseConfig(GLIS.display, GLIS.attrib_list, &GLIS.configs, 1, &GLIS.num_config)) {
        LOG_ERROR("eglChooseConfig() returned error %d", eglGetError());
        return false;
    }
    return true;
}

bool create_context(struct GLINITIALIZATIONSTRUCTURE & GLIS) {
    /*
Name
eglCreateContext — create a new EGL rendering context

C Specification
EGLContext eglCreateContext(
                             EGLDisplay display,
                             EGLConfig config,
                             EGLContext share_context,
                             EGLint const * attrib_list);

Parameters
display
    Specifies the EGL display connection.
config
    Specifies the EGL frame buffer configuration that defines the frame buffer resource available to the rendering context.
share_context
    Specifies another EGL rendering context with which to share data, as defined by the client API corresponding to the contexts. Data is also shared with all other contexts with which share_context shares data. EGL_NO_CONTEXT indicates that no sharing is to take place.
attrib_list
    Specifies attributes and attribute values for the context being created. Only the attribute EGL_CONTEXT_CLIENT_VERSION may be specified.

Description
eglCreateContext creates an EGL rendering context for the current rendering API (as set with eglBindAPI) and returns a handle to the context. The context can then be used to render into an EGL drawing surface. If eglCreateContext fails to create a rendering context, EGL_NO_CONTEXT is returned.
If share_context is not EGL_NO_CONTEXT, then all shareable data in the context (as defined by the client API specification for the current rendering API) are shared by context share_context, all other contexts share_context already shares with, and the newly created context. An arbitrary number of rendering contexts can share data. However, all rendering contexts that share data must themselves exist in the same address space. Two rendering contexts share an address space if both are owned by a single process.
attrib_list specifies a list of attributes for the context. The list has the same structure as described for eglChooseConfig. The attributes and attribute values which may be specified are as follows:
EGL_CONTEXT_MAJOR_VERSION
    Must be followed by an integer specifying the requested major version of an OpenGL or OpenGL ES context. The default value is 1. This attribute is an alias of the older EGL_CONTEXT_CLIENT_VERSION, and the tokens may be used interchangeably.
EGL_CONTEXT_MINOR_VERSION
    Must be followed by an integer specifying the requested minor version of an OpenGL or OpenGL ES context. The default value is 0.
EGL_CONTEXT_OPENGL_PROFILE_MASK
    Must be followed by an integer bitmask specifying the profile of an OpenGL context. Bits which may be set include EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT for a core profile and EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT for a compatibility profile. The default value is EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT. All OpenGL 3.2 and later implementations are required to implement the core profile, but implementation of the compatibility profile is optional.
EGL_CONTEXT_OPENGL_DEBUG
    Must be followed by EGL_TRUE, specifying that an OpenGL or OpenGL ES debug context should be created, or EGL_FALSE, if a non-debug context should be created. The default value is EGL_FALSE.
EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE
    Must be followed by EGL_TRUE, specifying that a forward-compatible OpenGL context should be created, or EGL_FALSE, if a non-forward-compatible context should be created. The default value is EGL_FALSE.
EGL_CONTEXT_OPENGL_ROBUST_ACCESS
    Must be followed by EGL_TRUE, specifying that an OpenGL or OpenGL ES context supporting robust buffer access should be created, or EGL_FALSE, if a non-robust context should be created. The default value is EGL_FALSE.
EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY
    Must be followed by EGL_LOSE_CONTEXT_ON_RESET, specifying that an OpenGL or OpenGL ES context with reset notification behavior GL_LOSE_CONTEXT_ON_RESET_ARB should be created, or EGL_NO_RESET_NOTIFICATION, specifying that an OpenGL or OpenGL ES context with reset notification behavior GL_NO_RESET_NOTIFICATION_ARB should be created, as described by the GL_ARB_robustness extension.
    If the EGL_CONTEXT_OPENGL_ROBUST_ACCESS attribute is not set to EGL_TRUE, context creation will not fail, but the resulting context may not support robust buffer access, and therefore may not support the requested reset notification strategy The default value for EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY is EGL_NO_RESET_NOTIFICATION .

There are many possible interactions between requested OpenGL and OpenGL ES context creation attributes, depending on the API versions and extensions supported by the implementation. These interactions are described in detail in the EGL 1.5 Specification, but are not listed here for compactness. The requested attributes may not be able to be satisfied, but context creation may still succeed. Applications should ensure that the OpenGL or OpenGL ES contexts supports needed features before using them, by determining the actual context version, supported extensions, and supported context flags using runtime queries.
     */
    if (!(GLIS.context = eglCreateContext(GLIS.display, GLIS.configs, nullptr, nullptr))) {
        LOG_ERROR("eglCreateContext() returned error %d", eglGetError());
        return false;
    }
    return true;
}

bool init_surface(struct GLINITIALIZATIONSTRUCTURE & GLIS) {
    /*
Name
eglCreateWindowSurface — create a new EGL window surface

C Specification
EGLSurface eglCreateWindowSurface(
                                   EGLDisplay display,
                                   EGLConfig config,
                                   NativeWindowType native_window,
                                   EGLint const * attrib_list);

Parameters
display
    Specifies the EGL display connection.
config
    Specifies the EGL frame buffer configuration that defines the frame buffer resource available to the surface.
native_window
    Specifies the native window.
attrib_list
    Specifies window surface attributes. May be NULL or empty (first attribute is EGL_NONE).

Description
eglCreateWindowSurface creates an on-screen EGL window surface and returns a handle to it. The behavior of eglCreateWindowSurface is identical to that of eglCreatePlatformWindowSurface except that the set of platforms to which display is permitted to belong, as well as the actual type of native_window, are implementation specific.

Errors
See errors for eglCreatePlatformWindowSurface.

Name
eglCreatePlatformWindowSurface — create a new EGL on-screen rendering surface

C Specification
EGLSurface eglCreatePlatformWindowSurface(
                                           EGLDisplay display,
                                           EGLConfig config,
                                           void * native_window,
                                           EGLAttrib const * attrib_list);

Parameters
display
    Specifies the EGL display connection.
config
    Specifies the EGL frame buffer configuration that defines the frame buffer resource available to the surface.
native_window
    Specifies the native window.
attrib_list
    Specifies a list of attributes for the window surface. May be NULL or empty (first attribute is EGL_NONE).

Description
eglCreatePlatformWindowSurface creates an on-screen EGL window surface and returns a handle to it. Any EGL context created with a compatible EGLConfig can be used to render into this surface.
If eglCreatePlatformWindowSurface fails to create a window surface, EGL_NO_SURFACE is returned.
native_window must belong to the same platform as display, and EGL considers the returned EGLSurface as belonging to that same platform. The EGL extension that defines the platform to which display belongs also defines the requirements for the native_window parameter.
Surface attributes are specified as a list of attribute-value pairs, terminated with EGL_NONE. Accepted attributes are:
EGL_GL_COLORSPACE
    Specifies the color space used by OpenGL and OpenGL ES when rendering to the surface. If its value is EGL_GL_COLORSPACE_SRGB, then a non-linear,
    perceptually uniform color space is assumed, with a corresponding GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING value of GL_SRGB. If its value is
    EGL_GL_COLORSPACE_LINEAR, then a linear color space is assumed, with a corresponding GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING value of
    GL_LINEAR. The default value of EGL_GL_COLORSPACE is EGL_GL_COLORSPACE_SRGB.

    Note that the EGL_GL_COLORSPACE attribute is used only by OpenGL and OpenGL ES contexts supporting sRGB framebuffers. EGL itself does not distinguish
    multiple colorspace models. Refer to the ``sRGB Conversion'' sections of the OpenGL 4.6 and OpenGL ES 3.2 Specifications for more information.
EGL_RENDER_BUFFER
    Specifies which buffer should be used for client API rendering to the window. If its value is EGL_SINGLE_BUFFER, then client APIs should render directly into
    the visible window. If its value is EGL_BACK_BUFFER, then client APIs should render into the back buffer. The default value of EGL_RENDER_BUFFER is
    EGL_BACK_BUFFER.

    Client APIs may not be able to respect the requested rendering buffer. To determine the actual buffer being rendered to by a context, call eglQueryContext.
EGL_VG_ALPHA_FORMAT
    Specifies how alpha values are interpreted by OpenVG when rendering to the surface. If its value is EGL_VG_ALPHA_FORMAT_NONPRE, then alpha values are
    not premultipled. If its value is EGL_VG_ALPHA_FORMAT_PRE, then alpha values are premultiplied. The default value of EGL_VG_ALPHA_FORMAT is
    EGL_VG_ALPHA_FORMAT_NONPRE.
EGL_VG_COLORSPACE
    Specifies the color space used by OpenVG when rendering to the surface. If its value is EGL_VG_COLORSPACE_sRGB, then a non-linear, perceptually uniform
    color space is assumed, with a corresponding VGImageFormat of form VG_s*. If its value is EGL_VG_COLORSPACE_LINEAR, then a linear color space is
    assumed, with a corresponding VGImageFormat of form VG_l*. The default value of EGL_VG_COLORSPACE is EGL_VG_COLORSPACE_sRGB.

Any EGL rendering context that was created with respect to config can be used to render into the surface. Use eglMakeCurrent to attach an EGL rendering context to the surface.
Use eglQuerySurface to retrieve the ID of config.
Use eglDestroySurface to destroy the surface.

Notes
Attribute EGL_GL_COLORSPACE is supported only if the EGL version is 1.5 or greater.
Attributes EGL_RENDER_BUFFER, EGL_VG_ALPHA_FORMAT, and EGL_VG_COLORSPACE, and the corresponding attribute values, are supported only if the EGL version is 1.2 or greater.
The EGL_VG_ALPHA_FORMAT and EGL_VG_COLORSPACE attributes are used only by OpenVG and EGL itself. Refer to section 11.2 of the OpenVG 1.0 specification for more information. The platform's use and interpretation of alpha values is outside the scope of EGL. However, the preferred behavior is for platforms to ignore the value of EGL_VG_ALPHA_FORMAT when compositing window surfaces.

Errors
If display and native_window do not belong to the same platform, then undefined behavior occurs.
EGL_NO_SURFACE is returned if creation of the surface fails.
EGL_BAD_DISPLAY is generated if display is not an EGL display connection.
EGL_NOT_INITIALIZED is generated if display has not been initialized.
EGL_BAD_CONFIG is generated if config is not a valid EGL frame buffer configuration.
EGL_BAD_NATIVE_WINDOW may be generated if native_window is not a valid native window for the same platform as display.
EGL_BAD_ATTRIBUTE is generated if attrib_list contains an invalid window attribute or if an attribute value is not recognized or is out of range.
EGL_BAD_ALLOC is generated if there is already an EGLSurface associated with native_window (as a result of a previous eglCreatePlatformWindowSurface call).
EGL_BAD_ALLOC is generated if the implementation cannot allocate resources for the new EGL window.
EGL_BAD_MATCH is generated if the pixel format of native_window does not correspond to the format, type, and size of the color buffers required by config. EGL_BAD_MATCH is generated if the pixel
EGL_BAD_MATCH is generated if config does not support rendering to windows (the EGL_SURFACE_TYPE attribute does not contain EGL_WINDOW_BIT).
EGL_BAD_MATCH is generated if config does not support the specified OpenVG alpha format attribute (the value of EGL_VG_ALPHA_FORMAT is EGL_VG_ALPHA_FORMAT_PRE and the EGL_VG_ALPHA_FORMAT_PRE_BIT is not set in the EGL_SURFACE_TYPE attribute of config) or colorspace attribute (the value of EGL_VG_COLORSPACE is EGL_VG_COLORSPACE_LINEAR and the EGL_VG_COLORSPACE_LINEAR_IT is not set in the EGL_SURFACE_TYPE attribute of config).
     */
    if (!(GLIS.surface = eglCreateWindowSurface(GLIS.display, GLIS.configs, GLIS.native_window, nullptr))) {
        LOG_ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
        return false;
    }
    return true;
}

bool switch_to_context(struct GLINITIALIZATIONSTRUCTURE & GLIS) {
    /*
Name
eglMakeCurrent — attach an EGL rendering context to EGL surfaces

C Specification
EGLBoolean eglMakeCurrent(
                       EGLDisplay display,
                       EGLSurface draw,
                       EGLSurface read,
                       EGLContext context);

Parameters
display
    Specifies the EGL display connection.
draw
    Specifies the EGL draw surface.
read
    Specifies the EGL read surface.
context
    Specifies the EGL rendering context to be attached to the surfaces.

Description
eglMakeCurrent binds context to the current rendering thread and to the draw and read surfaces.
For an OpenGL or OpenGL ES context, draw is used for all operations except for any pixel data read back or copied (glReadPixels, glCopyTexImage2D, and glCopyTexSubImage2D), which is taken from the frame buffer values of read. Note that the same EGLSurface may be specified for both draw and read.
For an OpenVG context, the same EGLSurface must be specified for both draw and read.
If the calling thread has already a current rendering context of the same client API type as context, then that context is flushed and marked as no longer current. context is then made the current context for the calling thread. For purposes of eglMakeCurrent, the client API type of all OpenGL ES and OpenGL contexts is considered the same. In other words, if any OpenGL ES context is currently bound and context is an OpenGL context, or if any OpenGL context is currently bound and context is an OpenGL ES context, the currently bound context will be made no longer current and context will be made current.
OpenGL and OpenGL ES buffer mappings created by e.g. glMapBuffer are not affected by eglMakeCurrent; they persist whether the context owning the buffer is current or not.
If draw is destroyed after eglMakeCurrent is called, then subsequent rendering commands will be processed and the context state will be updated, but the surface contents become undefined. If read is destroyed after eglMakeCurrent then pixel values read from the framebuffer (e.g., as result of calling glReadPixels) are undefined. If a native window or pixmap underlying the draw or read surfaces is destroyed, rendering and readback are handled as above.
To release the current context without assigning a new one, set context to EGL_NO_CONTEXT and set draw and read to EGL_NO_SURFACE . The currently bound context for the client API specified by the current rendering API is flushed and marked as no longer current, and there will be no current context for that client API after eglMakeCurrent returns. This is the only case in which eglMakeCurrent respects the current rendering API. In all other cases, the client API affected is determined by context. This is the only case where an uninitialized display may be passed to eglMakeCurrent.
If ctx is not EGL_NO_CONTEXT, then both draw and read must not be EGL_NO_SURFACE unless context is a context which supports being bound without read and draw surfaces. In this case the context is made current without a default framebuffer. The meaning of this is defined by the client API of the supporting context (see chapter 4 of the OpenGL 3.0 Specification, and the GL_OES_surfaceless_context OpenGL ES extension.).
The first time a OpenGL or OpenGL ES context is made current the viewport and scissor dimensions are set to the size of the draw surface (as though glViewport(0,0,w,h) and glScissor(0,0,w,h) were called, where w and h are the width and height of the surface, respectively). However, the viewport and scissor dimensions are not modified when context is subsequently made current. The client is responsible for resetting the viewport and scissor in this case.
The first time context is made current, if it is without a default framebuffer (e.g. both draw and read are EGL_NO_SURFACE ), then the viewport and scissor regions are set as though glViewport(0,0,0,0) and glScissor(0,0,0,0) were called.
Implementations may delay allocation of auxiliary buffers for a surface until they are required by a context (which may result in the EGL_BAD_ALLOC error described above). Once allocated, however, auxiliary buffers and their contents persist until a surface is deleted.
Use eglGetCurrentContext, eglGetCurrentDisplay, and eglGetCurrentSurface to query the current rendering context and associated display connection and surfaces.

Errors
If draw or read are not compatible with context, then an EGL_BAD_MATCH error is generated.
If context is current to some other thread, or if either draw or read are bound to contexts in another thread, an EGL_BAD_ACCESS error is generated.
If binding context would exceed the number of current contexts of that client API type supported by the implementation, an EGL_BAD_ACCESS error is generated.
If either draw or read are pbuffers created with eglCreatePbufferFromClientBuffer, and the underlying bound client API buffers are in use by the client API that created them, an EGL_BAD_ACCESS error is generated.
If context is not a valid context and is not EGL_NO_CONTEXT, an EGL_BAD_CONTEXT error is generated.
If either draw or read are not valid EGL surfaces and are not EGL_NO_SURFACE, an EGL_BAD_SURFACE error is generated.
If context is EGL_NO_CONTEXT and either draw or read are not EGL_NO_SURFACE, an EGL_BAD_MATCH error is generated.
If either of draw or read is a valid surface and the other is EGL_NO_SURFACE, an EGL_BAD_MATCH error is generated.
If context does not support being bound without read and draw surfaces, and both draw and read are EGL_NO_SURFACE, an EGL_BAD_MATCH error is generated.
If a native window underlying either draw or read is no longer valid, an EGL_BAD_NATIVE_WINDOW error is generated.
If draw and read cannot fit into graphics memory simultaneously, an EGL_BAD_MATCH error is generated.
If the previous context of the calling thread has unflushed commands, and the previous surface is no longer valid, an EGL_BAD_CURRENT_SURFACE error is generated.
If the ancillary buffers for draw and read cannot be allocated, an EGL_BAD_ALLOC error is generated.
If a power management event has occurred, an EGL_CONTEXT_LOST error is generated.
If any of the following are true:
context is not EGL_NO_CONTEXT
read is not EGL_NO_SURFACE
draw is not EGL_NO_SURFACE
then an EGL_NOT_INITIALIZED error is generated if display is a valid but uninitialized display.

As with other commands taking EGLDisplay parameters, if display is not a valid EGLDisplay handle, an EGL_BAD_DISPLAY error is generated. (Some implementations have chosen to allow EGL_NO_DISPLAY as a valid display parameter for eglMakeCurrent. This behavior is not portable to all EGL implementations, and should be considered as an undocumented vendor extension).
 */
    if (!eglMakeCurrent(GLIS.display, GLIS.surface, GLIS.surface, GLIS.context)) {
        LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
        return false;
    }
    return true;
}

bool get_width_height(struct GLINITIALIZATIONSTRUCTURE & GLIS) {
    /*
Name
eglQuerySurface — return EGL surface information

C Specification
EGLBoolean eglQuerySurface(
                        EGLDisplay display,
                        EGLSurface surface,
                        EGLint attribute,
                        EGLint * value);

Parameters
display
    Specifies the EGL display connection.
surface
    Specifies the EGL surface to query.
attribute
    Specifies the EGL surface attribute to be returned.
value
    Returns the requested value.

Description
eglQuerySurface returns in value the value of attribute for surface. attribute can be one of the following:
EGL_CONFIG_ID
    Returns the ID of the EGL frame buffer configuration with respect to which the surface was created.
EGL_GL_COLORSPACE
    Returns the color space used by OpenGL and OpenGL ES when rendering to the surface, either EGL_GL_COLORSPACE_SRGB or EGL_GL_COLORSPACE_LINEAR.
EGL_HEIGHT
    Returns the height of the surface in pixels.
EGL_HORIZONTAL_RESOLUTION
    Returns the horizontal dot pitch of the display on which a window surface is visible. The value returned is equal to the actual dot pitch, in pixels/meter, multiplied by the constant value EGL_DISPLAY_SCALING.
EGL_LARGEST_PBUFFER
    Returns the same attribute value specified when the surface was created with eglCreatePbufferSurface. For a window or pixmap surface, value is not modified.
EGL_MIPMAP_LEVEL
    Returns which level of the mipmap to render to, if texture has mipmaps.
EGL_MIPMAP_TEXTURE
    Returns EGL_TRUE if texture has mipmaps, EGL_FALSE otherwise.
EGL_MULTISAMPLE_RESOLVE
    Returns the filter used when resolving the multisample buffer. The filter may be either EGL_MULTISAMPLE_RESOLVE_DEFAULT or EGL_MULTISAMPLE_RESOLVE_BOX, as described for eglSurfaceAttrib.
EGL_PIXEL_ASPECT_RATIO
    Returns the aspect ratio of an individual pixel (the ratio of a pixel's width to its height). The value returned is equal to the actual aspect ratio multiplied by the constant value EGL_DISPLAY_SCALING.
EGL_RENDER_BUFFER
    Returns the buffer which client API rendering is requested to use. For a window surface, this is the same attribute value specified when the surface was created. For a pbuffer surface, it is always EGL_BACK_BUFFER. For a pixmap surface, it is always EGL_SINGLE_BUFFER. To determine the actual buffer being rendered to by a context, call eglQueryContext.
EGL_SWAP_BEHAVIOR
    Returns the effect on the color buffer when posting a surface with eglSwapBuffers. Swap behavior may be either EGL_BUFFER_PRESERVED or EGL_BUFFER_DESTROYED, as described for eglSurfaceAttrib.
EGL_TEXTURE_FORMAT
    Returns format of texture. Possible values are EGL_NO_TEXTURE, EGL_TEXTURE_RGB, and EGL_TEXTURE_RGBA.
EGL_TEXTURE_TARGET
    Returns type of texture. Possible values are EGL_NO_TEXTURE, or EGL_TEXTURE_2D.
EGL_VERTICAL_RESOLUTION
    Returns the vertical dot pitch of the display on which a window surface is visible. The value returned is equal to the actual dot pitch, in pixels/meter, multiplied by the constant value EGL_DISPLAY_SCALING.
EGL_VG_ALPHA_FORMAT
    Returns the interpretation of alpha values used by OpenVG when rendering to the surface, either EGL_VG_ALPHA_FORMAT_NONPRE or EGL_VG_ALPHA_FORMAT_PRE.
EGL_VG_COLORSPACE
    Returns the color space used by OpenVG when rendering to the surface, either EGL_VG_COLORSPACE_sRGB or EGL_VG_COLORSPACE_LINEAR.
EGL_WIDTH
    Returns the width of the surface in pixels.

Notes
Attribute EGL_GL_COLORSPACE is supported only if the EGL version is 1.5 or greater.
Attribute EGL_MULTISAMPLE_RESOLVE is supported only if the EGL version is 1.4 or greater.
Attributes EGL_DISPLAY_SCALING. EGL_HORIZONTAL_RESOLUTION, EGL_PIXEL_ASPECT_RATIO, EGL_RENDER_BUFFER, EGL_SWAP_BEHAVIOR, and EGL_VERTICAL_RESOLUTION are supported only if the EGL version is 1.2 or greater.
Querying attributes EGL_TEXTURE_FORMAT, EGL_TEXTURE_TARGET, EGL_MIPMAP_TEXTURE, or EGL_MIPMAP_LEVEL for a non-pbuffer surface is not an error, but value is not modified.
EGL_DISPLAY_SCALING is the constant value 10000. Floating-point values such as resolution and pixel aspect ratio are scaled by this value before being returned as integers so that sufficient precision to be meaningful will be retained in the returned value.
For an offscreen (pbuffer or pixmap) surface, or a surface whose pixel dot pitch or aspect ratio are unknown, querying EGL_HORIZONTAL_RESOLUTION, EGL_PIXEL_ASPECT_RATIO, or EGL_VERTICAL_RESOLUTION will return the constant value EGL_UNKNOWN (-1).

Errors
EGL_FALSE is returned on failure, EGL_TRUE otherwise. value is not modified when EGL_FALSE is returned.
EGL_BAD_DISPLAY is generated if display is not an EGL display connection.
EGL_NOT_INITIALIZED is generated if display has not been initialized.
EGL_BAD_SURFACE is generated if surface is not an EGL surface.
EGL_BAD_ATTRIBUTE is generated if attribute is not a valid surface attribute.
 */

    if (!eglQuerySurface(GLIS.display, GLIS.surface, EGL_WIDTH, &GLIS.width) ||
        !eglQuerySurface(GLIS.display, GLIS.surface, EGL_HEIGHT, &GLIS.height)) {
        LOG_ERROR("eglQuerySurface() returned error %d", eglGetError());
        return false;
    }
    return true;
}


#endif //GLNE_GLA_H
