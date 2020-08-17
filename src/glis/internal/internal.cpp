//
// Created by smallville7123 on 12/07/20.
//

#include <glis/glis.hpp>

#ifndef __ANDROID__
// X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifndef Status
#define Status int
#include <X11/Xresource.h>
#undef Status
#endif
#include <X11/Xresource.h>
// Wayland
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <glis/internal/xdg-shell-client-protocol.h>
#include <linux/input-event-codes.h>
#endif
#include <cstdlib>
#include <cassert>
#include <malloc.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>
#include <unistd.h>
#include <poll.h>

#include <sstream>
#include <queue>
#include <sys/time.h>
#include <glis/ipc/ipc.hpp>
#include <glis/internal/internal.hpp>


#define LOG_TAG "GLIS"

std::string GLIS_INTERNAL_MESSAGE_PREFIX = "";

bool GLIS_LOG_PRINT_NON_ERRORS = false;
bool GLIS_LOG_PRINT_VERTEX = false;
bool GLIS_LOG_PRINT_CONVERSIONS = false;
bool GLIS_LOG_PRINT_SHAPE_INFO = false;
bool GLIS_ABORT_ON_ERROR = false;
bool GLIS_ABORT_ON_DEBUG_LEVEL_API = false;

const char *GLIS_default_vertex_shader_source_RGB = R"glsl( #version 300 es
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;
    out vec3 ourColor;
    out vec2 TexCoord;
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
        TexCoord = aTexCoord;
    }
)glsl";

const char *GLIS_default_vertex_shader_source_RGBA = R"glsl( #version 300 es
    layout (location = 0) in vec2 texturePosition;
    layout (location = 1) in vec4 textureColor;
    layout (location = 2) in vec4 shaderColor;
    layout (location = 3) in vec2 textureCoordinates;
    out vec4 textureColorOut;
    out vec4 shaderColorOut;
    out vec4 textureCoordinatesOut;
    void main()
    {
        gl_Position = vec4(texturePosition, 1.0, 1.0);
        textureColorOut = textureColor;
        shaderColorOut = shaderColor;
        textureCoordinatesOut = vec4(textureCoordinates, 0.0, 1.0);
    }
)glsl";

const char *GLIS_default_fragment_shader_source_RGB = R"glsl( #version 300 es
    out highp vec4 FragColor;
    in highp vec3 ourColor;
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)glsl";

const char *GLIS_default_fragment_shader_source_RGBA = R"glsl( #version 300 es
    out highp vec4 FragColor;
    
    in highp vec4 textureColorOut;
    in highp vec4 shaderColorOut;
    in highp vec4 textureCoordinatesOut;
    void main()
    {
        FragColor = textureColorOut * shaderColorOut;
    }
)glsl";

const char *GLIS_default_texture_vertex_shader_source_RGB = R"glsl( #version 300 es
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    layout (location = 2) in vec2 aTexCoord;
    out vec4 ourColor;
    out vec2 TexCoord;
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = vec4(aColor, 1.0);
        TexCoord = aTexCoord;
    }
)glsl";

const char *GLIS_default_texture_fragment_shader_source_RGB = R"glsl( #version 300 es
    out highp vec4 FragColor;
    in highp vec4 ourColor;
    in highp vec2 TexCoord;
    uniform sampler2D texture1;
    void main()
    {
        FragColor = texture(texture1, TexCoord);
    }
)glsl";

const char *GLIS_default_texture_vertex_shader_source_RGBA = R"glsl( #version 300 es
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    layout (location = 2) in vec2 aTexCoord;
    out vec4 ourColor;
    out vec2 TexCoord;
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
        ourColor = aColor;
        TexCoord = aTexCoord;
    }
)glsl";

const char *GLIS_default_texture_fragment_shader_source_RGBA = R"glsl( #version 300 es
    out highp vec4 FragColor;
    in highp vec4 ourColor;
    in highp vec2 TexCoord;
    uniform sampler2D texture1;
    void main()
    {
        FragColor = texture(texture1, TexCoord);
    }
)glsl";

#define GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGING_FUNCTION, CASE_NAME, name, const, constSTRING, UNNAMED_STRING_CAN_PRINT_ERROR, UNNAMED_STRING_CANNOT_PRINT_ERROR, NAMED_STRING_CAN_PRINT_ERROR, NAMED_STRING_CANNOT_PRINT_ERROR, PRINT, IS_AN_ERROR) CASE_NAME: { \
    if(name == nullptr || name == nullptr || name == 0) { \
        if (PRINT) { \
            if ((UNNAMED_STRING_CAN_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += UNNAMED_STRING_CAN_PRINT_ERROR; \
                LOGGING_FUNCTION(msg.c_str(), constSTRING); \
            } \
        } \
        else { \
            if ((UNNAMED_STRING_CANNOT_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += UNNAMED_STRING_CANNOT_PRINT_ERROR; \
                LOGGING_FUNCTION("%s", msg.c_str()); \
            } \
        } \
    } \
    else { \
        if (PRINT) { \
            if ((NAMED_STRING_CAN_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += NAMED_STRING_CAN_PRINT_ERROR; \
                LOGGING_FUNCTION(msg.c_str(), name, constSTRING); \
            } \
        } \
        else { \
            if ((NAMED_STRING_CANNOT_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += NAMED_STRING_CANNOT_PRINT_ERROR; \
                LOGGING_FUNCTION(msg.c_str(), name); \
            } \
        } \
    } \
    if (IS_AN_ERROR) abort(); \
    break; \
}


#define GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOGGER, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGER, case const, name, const, constSTRING, nullptr, UNNAMED_STRING, nullptr, NAMED_STRING, false, false)

#define GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING(LOGGER, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING, IS_AN_ERROR) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGER, case const, name, const, constSTRING, UNNAMED_STRING, nullptr, NAMED_STRING, nullptr, true, IS_AN_ERROR)

#define GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING(name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING(LOG_ERROR, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING, true)

#define GLIS_ERROR_SWITCH_CASE(name, const) \
    GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING(name, const, #const, "%s", "%s generated error: %s")

#define GLIS_ERROR_SWITCH_CASE_DEFAULT(name, err) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOG_ERROR, default, name, err, err, "Unknown error: %d", "Unknown error", "%s generated an unknown error: %d", "%s generated an unknown error", true, true)

#define GLIS_boolean_to_string(val, TRUE_VALUE) val == TRUE_VALUE ? "true" : "false"

void GLIS::GLIS_error_to_string_GL(const char *name, GLint err) {
    GLIS_INTERNAL_MESSAGE_PREFIX = "OpenGL:          ";
    switch (err) {
        // GENERATED BY glGetError() ITSELF
        GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOG_INFO, name,
                                                                      GL_NO_ERROR,
                                                                      "GL_NO_ERROR",
                                                                      GLIS_LOG_PRINT_NON_ERRORS
                                                                      ? "no error was generated"
                                                                      : nullptr,
                                                                      GLIS_LOG_PRINT_NON_ERRORS
                                                                      ? "%s did not generate an error"
                                                                      : nullptr)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_ENUM)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_VALUE)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_OPERATION)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_FRAMEBUFFER_OPERATION)
        GLIS_ERROR_SWITCH_CASE(name, GL_OUT_OF_MEMORY)

        // WHEN ALL ELSE FAILS
        GLIS_ERROR_SWITCH_CASE_DEFAULT(name, err)
    }
    GLIS_INTERNAL_MESSAGE_PREFIX = "";
}

void GLIS::GLIS_error_to_string_EGL(const char *name, EGLint err) {
    GLIS_INTERNAL_MESSAGE_PREFIX = "OpenGL ES (EGL): ";
    switch (err) {
        // GENERATED BY eglGetError() ITSELF
        GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOG_INFO, name,
                                                                      EGL_SUCCESS,
                                                                      "EGL_SUCCESS",
                                                                      GLIS_LOG_PRINT_NON_ERRORS
                                                                      ? "no error was generated"
                                                                      : nullptr,
                                                                      GLIS_LOG_PRINT_NON_ERRORS
                                                                      ? "%s did not generate an error"
                                                                      : nullptr)
        GLIS_ERROR_SWITCH_CASE(name, EGL_NOT_INITIALIZED)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_ACCESS)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_ALLOC)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_ATTRIBUTE)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_CONTEXT)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_CONFIG)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_CURRENT_SURFACE)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_DISPLAY)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_SURFACE)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_MATCH)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_PARAMETER)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_NATIVE_PIXMAP)
        GLIS_ERROR_SWITCH_CASE(name, EGL_BAD_NATIVE_WINDOW)
        GLIS_ERROR_SWITCH_CASE(name, EGL_CONTEXT_LOST)

        // WHEN ALL ELSE FAILS
        GLIS_ERROR_SWITCH_CASE_DEFAULT(name, err)
    }
    GLIS_INTERNAL_MESSAGE_PREFIX = "";
}

void GLIS::GLIS_error_to_string_GL(GLint err) {
    GLIS_error_to_string_GL(nullptr, err);
}

void GLIS::GLIS_error_to_string_EGL(EGLint err) {
    GLIS_error_to_string_EGL(nullptr, err);
}

void GLIS::GLIS_error_to_string_GL() {
    GLIS_error_to_string_GL(glGetError());
}

void GLIS::GLIS_error_to_string_EGL() {
    GLIS_error_to_string_EGL(eglGetError());
}

void GLIS::GLIS_error_to_string() {
    GLIS_error_to_string_GL();
    GLIS_error_to_string_EGL();
}

void GLIS::GLIS_error_to_string_GL(const char *name) {
    GLIS_error_to_string_GL(name, glGetError());
}

void GLIS::GLIS_error_to_string_EGL(const char *name) {
    GLIS_error_to_string_EGL(name, eglGetError());
}

void GLIS::GLIS_error_to_string(const char *name) {
    GLIS_error_to_string_GL(name);
    GLIS_error_to_string_EGL(name);
}


void GLIS::GLIS_FORK(const char *__file, char *const *__argv) {
    errno = 0;

    LOG_INFO("LD_LIBRARY_PATH=%s", getenv("LD_LIBRARY_PATH"));
    pid_t pid = fork();
    LOG_ERROR("pid: %d", pid);
    if (pid == 0) {
        execvp(__file, __argv);
        LOG_ERROR("Cannot exec(%s) - %s", __file, strerror(errno));
        exit(1);
    }
}

void GLIS::GLIS_GL_INFORMATION() {
    const GLubyte *vendor = glGetString(GL_VENDOR);
    GLIS_error_to_string_GL("glGetString(GL_VENDOR)");
    LOG_INFO("GL_VENDOR: %s", vendor);

    const GLubyte *renderer = glGetString(GL_RENDERER);
    GLIS_error_to_string_GL("glGetString(GL_RENDERER)");
    LOG_INFO("GL_RENDERER: %s", renderer);

    const GLubyte *version = glGetString(GL_VERSION);
    GLIS_error_to_string_GL("glGetString(GL_VERSION)");
    LOG_INFO("GL_VERSION: %s", version);

    if (GLIS_ShaderCompilerSupported() == GL_TRUE) {
        const GLubyte *slv = glGetString(GL_SHADING_LANGUAGE_VERSION);
        GLIS_error_to_string_GL("glGetString(GL_SHADING_LANGUAGE_VERSION)");
        LOG_INFO("GL_SHADING_LANGUAGE_VERSION: %s", slv);
    } else {
        LOG_INFO("GL_SHADING_LANGUAGE_VERSION: (Not Supported)");
    }

    const GLubyte *extentions = glGetString(GL_EXTENSIONS);
    GLIS_error_to_string_GL("glGetString(GL_EXTENSIONS)");
    LOG_INFO("GL_EXTENSIONS: %s", extentions);
}

void GLIS::GLIS_EGL_INFORMATION(EGLDisplay &DISPLAY) {
    const char *client_apis = eglQueryString(DISPLAY, EGL_CLIENT_APIS);
    LOG_INFO("EGL_CLIENT_APIS: %s", client_apis);
    const char *vendor = eglQueryString(DISPLAY, EGL_VENDOR);
    LOG_INFO("EGL_VENDOR: %s", vendor);
    const char *version = eglQueryString(DISPLAY, EGL_VERSION);
    LOG_INFO("EGL_VERSION: %s", version);
    const char *extentions = eglQueryString(DISPLAY, EGL_EXTENSIONS);
    LOG_INFO("EGL_EXTENSIONS: %s", extentions);
}

bool GLIS::GLIS_initialize_display(class GLIS_CLASS &GLIS) {
    GLIS.display = eglGetDisplay(GLIS.display_id);
    GLIS_error_to_string_EGL("eglGetDisplay");
    if (GLIS.display == EGL_NO_DISPLAY) return false;
    GLIS.init_eglGetDisplay = true;
    EGLBoolean r = eglInitialize(GLIS.display, &GLIS.eglMajVers, &GLIS.eglMinVers);
    GLIS_error_to_string_EGL("eglInitialize");
    if (r == EGL_FALSE) return false;
    GLIS.init_eglInitialize = true;
    LOG_INFO("EGL initialized with version %d.%d", GLIS.eglMajVers, GLIS.eglMinVers);
    GLIS_EGL_INFORMATION(GLIS.display);
    return true;
}

bool GLIS::GLIS_initialize_configuration(class GLIS_CLASS &GLIS) {
    EGLBoolean r = eglChooseConfig(GLIS.display, GLIS.configuration_attributes, &GLIS.configuration, 1, &GLIS.number_of_configurations);
    GLIS_error_to_string_EGL("eglChooseConfig");
    if (r == EGL_FALSE) return false;
    GLIS.init_eglChooseConfig = true;
    return true;
}

bool GLIS::GLIS_initialize_surface_CreateWindowSurface(class GLIS_CLASS &GLIS) {
    if(GLIS.native_window == 0) {
        const char * msg = "error: a native window must be supplied prior to calling this function";
        LOG_ALWAYS_FATAL("%s", msg);
    }
    GLIS.surface = eglCreateWindowSurface(GLIS.display, GLIS.configuration, GLIS.native_window, nullptr);
    GLIS_error_to_string_EGL("eglCreateWindowSurface");
    if (GLIS.surface == EGL_NO_SURFACE) return false;
    GLIS.init_eglCreateWindowSurface = true;
    return true;
}

bool GLIS::GLIS_initialize_surface_CreatePbufferSurface(class GLIS_CLASS &GLIS) {
    GLIS.surface = eglCreatePbufferSurface(GLIS.display, GLIS.configuration, GLIS.surface_attributes);
    GLIS_error_to_string_EGL("eglCreatePbufferSurface");
    if (GLIS.surface == EGL_NO_SURFACE) return false;
    GLIS.init_eglCreatePbufferSurface = true;
    return true;
}

bool GLIS::GLIS_create_context(class GLIS_CLASS &GLIS) {
    GLIS.context = eglCreateContext(GLIS.display, GLIS.configuration, GLIS.shared_context, GLIS.context_attributes);
    GLIS_error_to_string_EGL("eglCreateContext");
    if (GLIS.context == EGL_NO_CONTEXT) return false;
    GLIS.init_eglCreateContext = true;
    return true;
}

bool GLIS::GLIS_switch_to_context(class GLIS_CLASS &GLIS) {
    EGLBoolean r = eglMakeCurrent(GLIS.display, GLIS.surface, GLIS.surface, GLIS.context);
    GLIS_error_to_string_EGL("eglMakeCurrent");
    if (r == EGL_FALSE) return false;
    GLIS.init_eglMakeCurrent = true;
    GLIS_error_to_string_GL("before GLINFO");
    GLIS_GL_INFORMATION();
    GLIS_error_to_string_GL("after GLINFO");
    return true;
}

bool GLIS::GLIS_get_width_height(class GLIS_CLASS &GLIS) {
    EGLBoolean r1 = eglQuerySurface(GLIS.display, GLIS.surface, EGL_WIDTH, &GLIS.surface_width);
    GLIS_error_to_string_EGL("eglQuerySurface");
    if (r1 == EGL_FALSE) return false;
    EGLBoolean r2 = eglQuerySurface(GLIS.display, GLIS.surface, EGL_HEIGHT, &GLIS.surface_height);
    GLIS_error_to_string_EGL("eglQuerySurface");
    if (r2 == EGL_FALSE) return false;
    return true;
}

void GLIS::on_gl_error(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                       const GLchar *message, const void *userParam) {
    std::stringstream MessageSS;

    MessageSS << "OpenGL debug message " << id << " (";
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            MessageSS << "Source: API.";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            MessageSS << "Source: Window System.";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            MessageSS << "Source: Shader Compiler.";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            MessageSS << "Source: Third Party.";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            MessageSS << "Source: Application.";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            MessageSS << "Source: Other.";
            break;
        default:
            MessageSS << "Source: Unknown (" << source << ").";
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            MessageSS << " Type: ERROR.";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            MessageSS << " Type: Deprecated Behaviour.";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            MessageSS << " Type: UNDEFINED BEHAVIOUR.";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            MessageSS << " Type: Portability.";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            MessageSS << " Type: PERFORMANCE.";
            break;
        case GL_DEBUG_TYPE_MARKER:
            MessageSS << " Type: Marker.";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            MessageSS << " Type: Push Group.";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            MessageSS << " Type: Pop Group.";
            break;
        case GL_DEBUG_TYPE_OTHER:
            MessageSS << " Type: Other.";
            break;
        default:
            MessageSS << " Type: Unknown (" << type << ").";
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            MessageSS << " Severity: HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            MessageSS << " Severity: Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            MessageSS << " Severity: Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            MessageSS << " Severity: Notification";
            break;
        default:
            MessageSS << " Severity: Unknown (" << severity << ")";
            break;
    }

    MessageSS << "): " << message;

    std::string s = MessageSS.str();
    const char * msg = s.c_str();
    if (GLIS_ABORT_ON_DEBUG_LEVEL_API && source == GL_DEBUG_SOURCE_API) LOG_ALWAYS_FATAL("%s", msg);
    LOG_ERROR("%s", msg);
}

void GLIS::enable_debug_callbacks(void) {
    glEnable(GL_DEBUG_OUTPUT);
    GLIS_error_to_string_GL("glEnable");
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    GLIS_error_to_string_GL("glEnable");
    glDebugMessageCallback(on_gl_error, nullptr);
    GLenum err = glGetError();
    GLIS_error_to_string_GL("glDebugMessageCallback", err);
    if (err != GL_NO_ERROR) {
        LOG_ERROR("Failed to enable debug messages");
    }
}

void GLIS::disable_debug_callbacks(void) {
    glDisable(GL_DEBUG_OUTPUT);
    GLIS_error_to_string_GL("glDisable");
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    GLIS_error_to_string_GL("glDisable");
    glDebugMessageCallback(nullptr, nullptr);
    GLenum err = glGetError();
    GLIS_error_to_string_GL("glDebugMessageCallback", err);
    if (err != GL_NO_ERROR) {
        LOG_ERROR("Failed to disable debug messages");
    }
}

bool GLIS::GLIS_initialize(class GLIS_CLASS &GLIS, GLint surface_type, bool debug) {
    if (GLIS.init_GLIS) return true;
    GLIS_error_to_string_GL("initializing");

    // this is how to debug native executables:
    // https://stackoverflow.com/questions/40492315/how-can-i-debug-an-android-native-executable-and-library-not-directly-integrated

    // TODO: https://github.com/fuyufjh/GraphicBuffer#how-to-use
    // Example for API >= 26. This repository is NOT needed,
    // because there is an open alternative in NDK [1].
    // The example does exactly the same thing as the one above.
    //
    // this uses EGLImage, however is is unclear how this could be used in an IPC situation
    //
    // http://www.jlekstrand.net/jason/projects/wayland/wayland-android/

    LOG_MAGNUM_DEBUG << "DEBUG MAGNUM TEST MESSAGE";
    LOG_MAGNUM_WARNING << "WARNING MAGNUM TEST MESSAGE";
    LOG_MAGNUM_ERROR << "ERROR MAGNUM TEST MESSAGE";

    LOG_INFO("Initializing");
    EGLBoolean r = eglBindAPI(EGL_OPENGL_ES_API);
    GLIS_error_to_string_EGL("eglBindAPI");
    if (r == EGL_FALSE) {
        return false;
    }

    LOG_INFO("Initializing display");
    if (!GLIS_initialize_display(GLIS)) {
        LOG_ERROR("Failed to initialize display");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Initialized display");
    GLIS_error_to_string_GL("stage");

    if (debug) {
        LOG_INFO("Debug mode enabled");
        const EGLint context_attributes[] = {
                EGL_CONTEXT_CLIENT_VERSION, GLIS.EGL_CONTEXT_CLIENT_VERSION_, EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR, EGL_NONE
        };
        GLIS.context_attributes = context_attributes;
        GLIS.debug_context = true;
    } else {
        LOG_INFO("Debug mode disabled");
        const EGLint context_attributes[] = {EGL_CONTEXT_CLIENT_VERSION, GLIS.EGL_CONTEXT_CLIENT_VERSION_, EGL_NONE};
        GLIS.context_attributes = context_attributes;
    }
    GLIS_error_to_string_GL("stage");

    LOG_INFO("Initializing configuration");
    if (!GLIS_initialize_configuration(GLIS)) {
        LOG_ERROR("Failed to initialize configuration");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Initialized configuration");
    GLIS_error_to_string_GL("stage");

    LOG_INFO("Initializing surface");
    if (surface_type == EGL_WINDOW_BIT) {
        LOG_INFO("Creating window surface");
        if (!GLIS_initialize_surface_CreateWindowSurface(GLIS)) {
            LOG_ERROR("Failed to initialize surface");
            GLIS_destroy_GLIS(GLIS);
            return false;
        }
    } else if (surface_type == EGL_PBUFFER_BIT) {
        LOG_INFO("creating pixel buffer surface");
        if (!GLIS_initialize_surface_CreatePbufferSurface(GLIS)) {
            LOG_ERROR("Failed to initialize surface");
            GLIS_destroy_GLIS(GLIS);
            return false;
        }
    }
    LOG_INFO("Initialized surface");
    GLIS_error_to_string_GL("stage");

    LOG_INFO("Initializing context");
    if (!GLIS_create_context(GLIS)) {
        LOG_ERROR("Failed to initialize context");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Initialized context");
    GLIS_error_to_string_GL("stage");

    LOG_INFO("Switching to context");
    if (!GLIS_switch_to_context(GLIS)) {
        LOG_ERROR("Failed to switch to context");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Switched to context");
    GLIS_error_to_string_GL("stage");

    // Magnum does not support glDebugMessageCallback
//    if (debug) {
//        LOG_INFO("Enabling debug callbacks");
//        enable_debug_callbacks();
//        LOG_INFO("Enabled debug callbacks");
//        GLIS.init_debug = true;
//    }

    LOG_INFO("Obtaining surface width and height");
    if (!GLIS_get_width_height(GLIS)) {
        LOG_ERROR("Failed to obtain surface width and height");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Obtained surface width and height");
    GLIS_error_to_string_GL("stage");

    GLIS.init_GLIS = true;
    LOG_INFO("Initialized");
    GLIS_error_to_string_GL("initialized");
    return true;
}

void GLIS::GLIS_destroy_GLIS(class GLIS_CLASS &GLIS) {
    if (!GLIS.init_GLIS) return;
    LOG_INFO("Uninitializing");

    if (GLIS.init_debug) {
        LOG_INFO("Disabling debug callbacks");
        disable_debug_callbacks();
        LOG_INFO("Disabled debug callbacks");
        GLIS.init_debug = false;
    }
    if (GLIS.init_eglMakeCurrent) {
        LOG_INFO("Switching context to no context");
        eglMakeCurrent(GLIS.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        GLIS_error_to_string_EGL("eglMakeCurrent");
        GLIS.init_eglMakeCurrent = false;
    }
    if (GLIS.init_eglCreateContext) {
        LOG_INFO("Uninitializing context");
        eglDestroyContext(GLIS.display, GLIS.context);
        GLIS_error_to_string_EGL("eglDestroyContext");
        GLIS.context = EGL_NO_CONTEXT;
        GLIS.shared_context = EGL_NO_CONTEXT;
        GLIS.init_eglCreateContext = false;
    }
    if (GLIS.init_eglCreateWindowSurface || GLIS.init_eglCreatePbufferSurface) {
        LOG_INFO("Uninitializing surface");
        eglDestroySurface(GLIS.display, GLIS.surface);
        GLIS_error_to_string_EGL("eglDestroySurface");
        GLIS.surface = EGL_NO_SURFACE;
        GLIS.init_eglCreateWindowSurface = false;
        GLIS.init_eglCreatePbufferSurface = false;
    }
    if (GLIS.init_eglChooseConfig) {
        // TODO: figure how to undo init_eglChooseConfig
    }
    if (GLIS.init_eglInitialize) {
        LOG_INFO("Uninitializing display");
        eglTerminate(GLIS.display);
        GLIS_error_to_string_EGL("eglTerminate");
        GLIS.init_eglInitialize = false;
    }
    if (GLIS.init_eglGetDisplay) {
        LOG_INFO("Setting display to no display");
        GLIS.display = EGL_NO_DISPLAY;
        GLIS.init_eglGetDisplay = false;
    }
    GLIS.init_GLIS = false;
    LOG_INFO("Uninitialized");
}

bool GLIS::GLIS_setupOnScreenRendering(class GLIS_CLASS &GLIS, EGLContext shared_context) {
    GLIS.shared_context = shared_context;

    const EGLint config[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE};
    GLIS.configuration_attributes = config;

    const EGLint surface[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_BLUE_SIZE, 8,
                              EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_ALPHA_SIZE, 8,
                              EGL_DEPTH_SIZE, 16, EGL_NONE};
    GLIS.surface_attributes = surface;

    return GLIS_initialize(GLIS, EGL_WINDOW_BIT, true);
}

bool GLIS::GLIS_setupOnScreenRendering(class GLIS_CLASS &GLIS) {
    return GLIS_setupOnScreenRendering(GLIS, EGL_NO_CONTEXT);
}

bool
GLIS::GLIS_setupOffScreenRendering(class GLIS_CLASS &GLIS, int w, int h, EGLContext shared_context) {
    if (IPC == IPC_MODE.shared_memory) if (!GLIS_INIT_SHARED_MEMORY(w, h)) return false;

    GLIS.shared_context = shared_context;

    const EGLint config[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE};
    GLIS.configuration_attributes = config;

    const EGLint surface[] = {EGL_WIDTH, w, EGL_HEIGHT, h, EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB,
                              EGL_TEXTURE_TARGET, EGL_TEXTURE_2D, EGL_NONE};
    GLIS.surface_attributes = surface;

    return GLIS_initialize(GLIS, EGL_PBUFFER_BIT, true);
}

bool GLIS::GLIS_setupOffScreenRendering(class GLIS_CLASS &GLIS, int w, int h) {
    return GLIS_setupOffScreenRendering(GLIS, w, h, EGL_NO_CONTEXT);
}

GLboolean GLIS::GLIS_ShaderCompilerSupported() {
    GLboolean GLSC_supported;
    glGetBooleanv(GL_SHADER_COMPILER, &GLSC_supported);
    GLIS_error_to_string_GL("glGetBooleanv(GL_SHADER_COMPILER, &GLSC_supported)");
    LOG_INFO("Supports Shader Compiler: %s", GLIS_boolean_to_string(GLSC_supported, GL_TRUE));
    return GLSC_supported;
}

GLuint GLIS::GLIS_createShader(GLenum shaderType, const char *&src) {
    if (GLIS_ShaderCompilerSupported()) {
        const char *SHADER_TYPE = nullptr;
        switch (shaderType) {
            case GL_COMPUTE_SHADER:
                SHADER_TYPE = "Compute";
                break;
            case GL_FRAGMENT_SHADER:
                SHADER_TYPE = "Fragment";
                break;
            case GL_GEOMETRY_SHADER:
                SHADER_TYPE = "Geometry";
                break;
            case GL_VERTEX_SHADER:
                SHADER_TYPE = "Vertex";
                break;
            default:
                SHADER_TYPE = "Unknown";
                break;
        }
        LOG_INFO("Creating %s Shader", SHADER_TYPE);
        GLuint shader = glCreateShader(shaderType);
        GLIS_error_to_string_GL("glCreateShader");
        if (!shader) {
            return 0;
        }
        glShaderSource(shader, 1, &src, nullptr);
        GLIS_error_to_string_GL("glShaderSource");
        LOG_INFO("Created %s Shader", SHADER_TYPE);
        GLint compiled = GL_FALSE;
        LOG_INFO("Compiling %s Shader", SHADER_TYPE);
        glCompileShader(shader);
        GLIS_error_to_string_GL("glCompileShader");
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        GLIS_error_to_string_GL("glGetShaderiv");
        if (compiled != GL_TRUE) {
            GLint infoLogLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
            GLIS_error_to_string_GL("glGetShaderiv");
            if (infoLogLen > 0) {
                GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
                if (infoLog) {

                    glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog);
                    GLIS_error_to_string_GL("glGetShaderInfoLog");
                    LOG_ERROR("Could not compile %s shader:\n%s", SHADER_TYPE, infoLog);
                    free(infoLog);
                }
            }
            glDeleteShader(shader);
            GLIS_error_to_string_GL("glDeleteShader");
            return 0;
        }
        assert(glIsShader(shader) == GL_TRUE);
        LOG_INFO("Compiled %s Shader", SHADER_TYPE);
        return shader;
    } else return 0;
}

GLboolean GLIS::GLIS_validate_program_link(GLuint &Program) {
    GLint linked = GL_FALSE;
    glGetProgramiv(Program, GL_LINK_STATUS, &linked);
    GLIS_error_to_string_GL("glGetProgramiv");
    if (linked != GL_TRUE) {
        GLint infoLogLen = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLen);
        GLIS_error_to_string_GL("glGetProgramiv");
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {

                glGetProgramInfoLog(Program, infoLogLen, nullptr, infoLog);
                GLIS_error_to_string_GL("glGetProgramInfoLog");
                LOG_ERROR("Could not link program:\n%s", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(Program);
        GLIS_error_to_string_GL("glDeleteProgram");
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean GLIS::GLIS_validate_program_valid(GLuint &Program) {
    GLint validated = GL_FALSE;
    glValidateProgram(Program);
    GLIS_error_to_string_GL("glValidateProgram");
    glGetProgramiv(Program, GL_VALIDATE_STATUS, &validated);
    GLIS_error_to_string_GL("glGetProgramiv");
    if (validated != GL_TRUE) {
        GLint infoLogLen = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLen);
        GLIS_error_to_string_GL("glGetProgramiv");
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {

                glGetProgramInfoLog(Program, infoLogLen, nullptr, infoLog);
                GLIS_error_to_string_GL("glGetProgramInfoLog");
                LOG_ERROR("Could not validate program:\n%s", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(Program);
        GLIS_error_to_string_GL("glDeleteProgram");
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean GLIS::GLIS_validate_program(GLuint &Program) {
    if (GLIS_validate_program_link(Program) == GL_TRUE)
        if (GLIS_validate_program_valid(Program) == GL_TRUE) {
            GLboolean v = glIsProgram(Program);
            GLIS_error_to_string_GL("glIsProgram");
            return v;
        }
    return GL_FALSE;
}

void GLIS::GLIS_build_simple_shader_program(
        GLuint & vertexShader, const char *vertexSource,
        GLuint & fragmentShader, const char *fragmentSource,
        GLuint & shaderProgram
) {
    vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
    fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
    LOG_INFO("Creating Shader program");
    shaderProgram = glCreateProgram();
    GLIS_error_to_string_GL("glCreateProgram");
    LOG_INFO("Created Shader program");
    LOG_INFO("Attaching vertex Shader to program");
    glAttachShader(shaderProgram, vertexShader);
    GLIS_error_to_string_GL("glAttachShader");
    LOG_INFO("Attached vertex Shader to program");
    LOG_INFO("Attaching fragment Shader to program");
    glAttachShader(shaderProgram, fragmentShader);
    GLIS_error_to_string_GL("glAttachShader");
    LOG_INFO("Attached fragment Shader to program");
    LOG_INFO("Linking Shader program");
    glLinkProgram(shaderProgram);
    GLIS_error_to_string_GL("glLinkProgram");
    LOG_INFO("Linked Shader program");
    LOG_INFO("Validating Shader program");
    GLboolean ProgramIsValid = GLIS_validate_program(shaderProgram);
    assert(ProgramIsValid == GL_TRUE);
    LOG_INFO("Validated Shader program");
}

void GLIS::GLIS_build_simple_shader_program(
        GLuint & vertexShader,
        GLuint & fragmentShader,
        GLuint & shaderProgram
) {
    GLIS_build_simple_shader_program_RGB(
            vertexShader,
            fragmentShader,
            shaderProgram
    );
};

void GLIS::GLIS_build_simple_shader_program_RGB(
        GLuint & vertexShader,
        GLuint & fragmentShader,
        GLuint & shaderProgram
) {
    GLIS_build_simple_shader_program(
            vertexShader, GLIS_default_vertex_shader_source_RGB,
            fragmentShader, GLIS_default_fragment_shader_source_RGB,
            shaderProgram
    );
};

void GLIS::GLIS_build_simple_shader_program_RGBA(
        GLuint & vertexShader,
        GLuint & fragmentShader,
        GLuint & shaderProgram
) {
    GLIS_build_simple_shader_program(
            vertexShader, GLIS_default_vertex_shader_source_RGBA,
            fragmentShader, GLIS_default_fragment_shader_source_RGBA,
            shaderProgram
    );
};

void GLIS::GLIS_build_simple_texture_shader_program(
        GLuint & vertexShader,
        GLuint & fragmentShader,
        GLuint & shaderProgram
) {
    GLIS_build_simple_texture_shader_program_RGB(
            vertexShader,
            fragmentShader,
            shaderProgram
    );
};

void GLIS::GLIS_build_simple_texture_shader_program_RGB(
        GLuint & vertexShader,
        GLuint & fragmentShader,
        GLuint & shaderProgram
) {
    GLIS_build_simple_shader_program(
            vertexShader, GLIS_default_texture_vertex_shader_source_RGB,
            fragmentShader, GLIS_default_texture_fragment_shader_source_RGB,
            shaderProgram
    );
};

void GLIS::GLIS_build_simple_texture_shader_program_RGBA(
        GLuint & vertexShader,
        GLuint & fragmentShader,
        GLuint & shaderProgram
) {
    GLIS_build_simple_shader_program(
            vertexShader, GLIS_default_texture_vertex_shader_source_RGBA,
            fragmentShader, GLIS_default_texture_fragment_shader_source_RGBA,
            shaderProgram
    );
};

void GLIS::GLIS_set_conversion_origin(int origin) {
    GLIS_CONVERSION_ORIGIN = origin;
    if (GLIS_LOG_PRINT_CONVERSIONS)
        switch (GLIS_CONVERSION_ORIGIN) {
            case GLIS_CONVERSION_ORIGIN_TOP_LEFT:
                LOG_INFO("converting with origin top left");
                break;
            case GLIS_CONVERSION_ORIGIN_TOP_RIGHT:
                LOG_INFO("converting with origin top right");
                break;
            case GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT:
                LOG_INFO("converting with origin bottom left");
                break;
            case GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT:
                LOG_INFO("converting with origin bottom right");
                break;
            default:
                LOG_INFO("unknown conversion");
                break;
        }
}

void GLIS::GLIS_set_texture(GLenum textureUnit, GLuint & texture) {
    glActiveTexture(textureUnit);
    GLIS_error_to_string_GL("glActiveTexture");
    glBindTexture(GL_TEXTURE_2D, texture);
    GLIS_error_to_string_GL("glBindTexture");
}

void GLIS::GLIS_set_framebuffer(GLuint &framebuffer, GLuint &renderbuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GLIS_error_to_string_GL("glBindFramebuffer");
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    GLIS_error_to_string_GL("glBindRenderbuffer");
}

void GLIS::GLIS_set_default_framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLIS_error_to_string_GL("glBindFramebuffer");
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    GLIS_error_to_string_GL("glBindRenderbuffer");
}

void GLIS::GLIS_set_default_texture(GLenum textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLIS::GLIS_framebuffer(GLuint &framebuffer, GLuint &renderbuffer,
                            const GLint &texture_width, const GLint &texture_height) {
    glGenRenderbuffers(1, &renderbuffer);
    GLIS_error_to_string_GL("glGenRenderbuffers");
    GLIS_set_framebuffer(framebuffer, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8UI, texture_width, texture_height);
    GLIS_error_to_string_GL("glRenderbufferStorage");

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
    GLIS_error_to_string_GL("glFramebufferRenderbuffer");
}

void GLIS::GLIS_texture(GLuint &texture) {
    glGenTextures(1, &texture); GLIS_error_to_string_GL("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, texture); GLIS_error_to_string_GL("glBindTexture");
}

void GLIS::GLIS_texture_buffer(GLuint &framebuffer, GLuint &renderbuffer, GLuint &texture,
                               const GLint &texture_width, const GLint &texture_height) {
    // gen renderbuffer to remain temporarily compatible with code that expects current behavour:
    // at cleanup they call glDeleteRenderbuffers(1, &renderbuffer);
    glGenRenderbuffers(1, &renderbuffer);

    glGenFramebuffers(1, &framebuffer); GLIS_error_to_string_GL("glGenFramebuffers");
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); GLIS_error_to_string_GL("glBindFramebuffer");

    glGenTextures(1, &texture); GLIS_error_to_string_GL("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, texture); GLIS_error_to_string_GL("glBindTexture");
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, texture_width, texture_height); GLIS_error_to_string_GL("glTexStorage2D");
    // glGenerateMipmap DOES NOT affect the total size of glReadPixels
    glGenerateMipmap(GL_TEXTURE_2D); GLIS_error_to_string_GL("glGenerateMipmap");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); GLIS_error_to_string_GL("glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GLIS_error_to_string_GL("glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); GLIS_error_to_string_GL("glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); GLIS_error_to_string_GL("glTexParameteri");

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0); GLIS_error_to_string_GL("glFramebufferTexture2D");
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); GLIS_error_to_string_GL("glDrawBuffers");

    // framebuffer is complete
    GLenum FramebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER); GLIS_error_to_string_GL("glCheckFramebufferStatus");
    if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("framebuffer is not complete");
    else LOG_INFO("framebuffer is complete");
}

void GLIS::GLIS_Sync_GPU() {
//    LOG_INFO("synchronizing with GPU");
    double start = now_ms();
    GLsync GPU = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    GLIS_error_to_string_GL("glFenceSync");
    if (GPU == nullptr) LOG_ERROR("glFenceSync failed");
//    LOG_INFO("synchronizing");
    glWaitSync(GPU, 0, GL_TIMEOUT_IGNORED);
    GLIS_error_to_string_GL("glWaitSync");
//    LOG_INFO("synchronized");
    glDeleteSync(GPU);
    GLIS_error_to_string_GL("glDeleteSync");
    double end = now_ms();
//    LOG_INFO("synchronized with GPU in %G milliseconds", end - start);
}

EGLBoolean GLIS::GLIS_SwapBuffers(class GLIS_CLASS &GLIS) {
    EGLBoolean x = eglSwapBuffers(GLIS.display, GLIS.surface);
    GLIS_error_to_string_EGL("eglSwapBuffers");
    return x;
}

EGLBoolean GLIS::vsync(class GLIS_CLASS &GLIS, EGLint interval) {
    EGLBoolean x = eglSwapInterval(GLIS.display, interval);
    GLIS_error_to_string_EGL("eglSwapInterval");
    return x;
};

void GLIS::GLIS_resize(GLuint **TEXDATA, size_t &TEXDATA_LEN, int width_from, int height_from,
                       int width_to, int height_to) {
    GLIS_BACKUP backup;
    // save
    backup.backup();
    // RESIZE TEXTURE
    
    GLuint FB;
    GLuint RB;
    GLuint texture;
    GLIS_texture_buffer(FB, RB, texture, width_to, height_to);
    GLuint CHILDshaderProgram;
    GLuint CHILDvertexShader;
    GLuint CHILDfragmentShader;
    GLIS_build_simple_shader_program(CHILDvertexShader, CHILDfragmentShader, CHILDshaderProgram);
    LOG_INFO("Using Shader program");
    glUseProgram(CHILDshaderProgram);
    LOG_INFO("drawing rectangle");
    GLIS_draw_rectangle<GLint>(
            GL_TEXTURE0, texture, 0, 0, 0, width_to, height_to, width_from,
            height_from);
    LOG_INFO("drawn rectangle");
    TEXDATA_LEN = width_to * height_to * sizeof(GLuint);
    *TEXDATA = new GLuint[TEXDATA_LEN];
    memset(*TEXDATA, 0, TEXDATA_LEN);
    glReadPixels(0, 0, width_to, height_to, GL_RGBA8, GL_UNSIGNED_BYTE,
                 *TEXDATA);
    glDeleteProgram(CHILDshaderProgram);
    glDeleteShader(CHILDfragmentShader);
    glDeleteShader(CHILDvertexShader);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &RB);
    glDeleteFramebuffers(1, &FB);
    // restore
    backup.restore();
}

void GLIS::GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(GLIS::GLIS_shared_memory &shared_memory, int w,
                                                    int h) {
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS(shared_memory.slot.total_size, shared_memory.slot.status, sizeof(int8_t));
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.command, sizeof(int8_t), shared_memory.slot.status);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int8_t, sizeof(int8_t), shared_memory.slot.command);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_0.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_0.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_0.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_0.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_0.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_0.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_1.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_1.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_1.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_1.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_1.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_1.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_2.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_2.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_2.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_2.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_2.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_2.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_3.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_3.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_3.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_3.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_3.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_3.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int16_t, sizeof(int16_t), shared_memory.slot.additional_data_4.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int32_t, sizeof(int32_t), shared_memory.slot.additional_data_4.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_int64_t, sizeof(int64_t), shared_memory.slot.additional_data_4.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.additional_data_4.type_size_t, sizeof(size_t), shared_memory.slot.additional_data_4.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int8_t, sizeof(int8_t), shared_memory.slot.additional_data_4.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_0.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_0.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_0.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_0.type_size_t, sizeof(size_t), shared_memory.slot.result_data_0.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_0.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_1.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_1.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_1.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_1.type_size_t, sizeof(size_t), shared_memory.slot.result_data_1.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_1.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_2.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_2.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_2.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_2.type_size_t, sizeof(size_t), shared_memory.slot.result_data_2.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_2.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_3.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_3.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_3.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_3.type_size_t, sizeof(size_t), shared_memory.slot.result_data_3.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int8_t, sizeof(int8_t), shared_memory.slot.result_data_3.type_size_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int16_t, sizeof(int16_t), shared_memory.slot.result_data_4.type_int8_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int32_t, sizeof(int32_t), shared_memory.slot.result_data_4.type_int16_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_int64_t, sizeof(int64_t), shared_memory.slot.result_data_4.type_int32_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.result_data_4.type_size_t, sizeof(size_t), shared_memory.slot.result_data_4.type_int64_t);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTM(shared_memory.slot.total_size, shared_memory.slot.texture, (sizeof(GLuint) * w * h), shared_memory.slot.result_data_4.type_size_t);
}

bool GLIS::GLIS_shared_memory_open(GLIS::GLIS_shared_memory &sh) {
    return SHM_open(sh.fd, &sh.data, sh.size);
}

void GLIS::GLIS_shared_memory_clear(GLIS::GLIS_shared_memory &sh) {
    memset(sh.data, 0, sh.size);
}

bool GLIS::GLIS_shared_memory_malloc(GLIS::GLIS_shared_memory &sh, size_t size) {
    sh.size = size;
    return SHM_create(sh.fd, &sh.data, sh.size);
}

bool GLIS::GLIS_shared_memory_realloc(GLIS::GLIS_shared_memory &sh, size_t size) {
    return SHM_resize(sh.fd, &sh.data, size);
}

bool GLIS::GLIS_shared_memory_free(GLIS::GLIS_shared_memory &sh) {
    if (SHM_close(sh.fd, &sh.data, sh.size)) {
        sh.size = 0;
        return true;
    }
    return false;
}

void GLIS::GLIS_shared_memory_increase_reference(GLIS::GLIS_shared_memory &shared_memory) {
    shared_memory.reference_count++;
}

void *GLIS::KEEP_ALIVE_MAIN_NOTIFIER(void *arg) {
    int *ret = new int;
    assert(arg != nullptr);
    class Client {
    public:
        class GLIS_shared_memory shared_memory;
        SOCKET_SERVER * server = nullptr;
        size_t server_id = 0;
        size_t id = -1;
        size_t table_id = 0;
        bool connected = false;
    };
    Client * client = static_cast<Client *>(arg);
    SOCKET_SERVER *server = SERVER_get(client->table_id);
    LOG_ERROR("CLIENT ID: %zu, connecting", client->id);
    assert(server->socket_accept());
    LOG_ERROR("CLIENT ID: %zu, connected", client->id);
    client->connected = true;
    server->connection_wait_until_disconnect();
    LOG_INFO("CLIENT ID: %zu, closed its connection", client->id);
    SERVER_deallocate_server(client->table_id);
    GLIS_shared_memory_free(client->shared_memory);
    LOG_INFO("CLIENT ID: %zu, shared_memory.reference_count = %zu", client->id, client->shared_memory.reference_count);
    client->shared_memory.reference_count--;
    LOG_INFO("CLIENT ID: %zu, shared_memory.reference_count = %zu", client->id, client->shared_memory.reference_count);
    *ret = 0;
    return ret;
}

bool GLIS::GLIS_start_drawing() {
    SERVER_LOG_TRANSFER_INFO = true;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.start_drawing);
    if (GLIS_CLIENT.socket_put_serial(cmd)) {
        if (GLIS_CLIENT.socket_get_serial(server)) {
            bool ret = false;
            server.get<bool>(&ret);
            return ret == true;
        } else
            LOG_ERROR("failed to get serial from the server");
    } else
        LOG_ERROR("failed to send command to the server");
    return false;
}

bool GLIS::GLIS_stop_drawing() {
    SERVER_LOG_TRANSFER_INFO = true;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.stop_drawing);
    if (GLIS_CLIENT.socket_put_serial(cmd)) {
        if (GLIS_CLIENT.socket_get_serial(server)) {
            bool ret = false;
            server.get<bool>(&ret);
            return ret == true;
        } else
            LOG_ERROR("failed to get serial from the server");
    } else
        LOG_ERROR("failed to send command to the server");
    return false;
}

void GLIS::GLIS_sync_server(const char *operation, size_t id) {
    LOG_ERROR("waiting for %s (for window id %zu) to complete on server side", operation, id);
    GLIS_INTERNAL_SHARED_MEMORY.slot.status.store_int8_t(GLIS_INTERNAL_SHARED_MEMORY.status.ready_to_be_read);
    while (GLIS_INTERNAL_SHARED_MEMORY.slot.status.load_int8_t() == GLIS_INTERNAL_SHARED_MEMORY.status.ready_to_be_read);
    LOG_ERROR("%s (for window id %zu) has completed on server side", operation, id);
}

bool GLIS::GLIS_INIT_SHARED_MEMORY(int w, int h) {
    if (GLIS_SHARED_MEMORY_INITIALIZED) return true;

    SERVER_LOG_TRANSFER_INFO = true;
    SOCKET_CLIENT client;
    serializer cmd;
    serializer server;
    cmd.add<int>(GLIS_SERVER_COMMANDS.new_connection);
    GLIS_SHARED_MEMORY_SLOTS_COMPUTE_SLOTS__(GLIS_INTERNAL_SHARED_MEMORY, w, h);
    assert(GLIS_shared_memory_malloc(
            GLIS_INTERNAL_SHARED_MEMORY, GLIS_INTERNAL_SHARED_MEMORY.slot.total_size
    ));
    assert(ashmem_valid(GLIS_INTERNAL_SHARED_MEMORY.fd));
    GLIS_shared_memory_increase_reference(GLIS_INTERNAL_SHARED_MEMORY);
    cmd.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY.size);
    cmd.add<size_t>(GLIS_INTERNAL_SHARED_MEMORY.reference_count);
    cmd.add<int>(w);
    cmd.add<int>(h);
    if (client.connect_to_server()) {
        if (client.socket_put_serial(cmd)) {
            client.socket_put_fd(GLIS_INTERNAL_SHARED_MEMORY.fd);
            if (client.socket_get_serial(server)) {
                if (client.disconnect_from_server()) {
                    char *server_name;
                    server.get_raw_pointer<char>(&server_name);
                    KEEP_ALIVE.set_name(server_name);
                    delete[] server_name;
                    LOG_ERROR("connecting to keep alive server");
                    if (KEEP_ALIVE.connect_to_server()) {
                        GLIS_sync_server("GLIS_INIT_SHARED_MEMORY", -1);
                        bool ret = false;
                        server.get<bool>(&ret);
                        if (ret == true) {
                            GLIS_SHARED_MEMORY_INITIALIZED = true;
                            return true;
                        } else
                            LOG_ERROR("failed to initialize shared memory");
                    } else
                        LOG_ERROR("failed to connect to server");
                } else
                    LOG_ERROR("failed to disconnect from the server");
            } else
                LOG_ERROR("failed to get serial from the server");
        } else
            LOG_ERROR("failed to send command to the server");
    } else
        LOG_ERROR("failed to connect to server");
    return false;
}

size_t GLIS::GLIS_new_window(int x, int y, int w, int h) {
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.new_window);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_int64_t.store_int64_t(x);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_1.type_int64_t.store_int64_t(y);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_2.type_int64_t.store_int64_t(x + w);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_3.type_int64_t.store_int64_t(y + h);
    GLIS_sync_server("GLIS_new_window", -1);
    return GLIS_INTERNAL_SHARED_MEMORY.slot.result_data_0.type_size_t.load_size_t();
}

bool GLIS::GLIS_modify_window(size_t window_id, int x, int y, int w, int h) {
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.modify_window);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_int64_t.store_int64_t(x);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_1.type_int64_t.store_int64_t(y);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_2.type_int64_t.store_int64_t(x + w);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_3.type_int64_t.store_int64_t(y + h);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_4.type_size_t.store_size_t(window_id);
    GLIS_sync_server("GLIS_modify_window", window_id);
    return true;
}

bool GLIS::GLIS_close_window(size_t window_id) {
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.close_window);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_size_t.store_size_t(window_id);
    GLIS_sync_server("GLIS_close_window", window_id);
    return true;
}

void GLIS::GLIS_upload_texture(GLIS_CLASS &GLIS, size_t &window_id, GLuint &texture_id,
                               const GLint &texture_width, const GLint &texture_height) {
    LOG_INFO("uploading texture");
    GLIS_SwapBuffers(GLIS);
    GLIS_Sync_GPU();
    GLIS_INTERNAL_SHARED_MEMORY.slot.command.store_int8_t(GLIS_SERVER_COMMANDS.texture);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_0.type_int64_t.store_int64_t(texture_width);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_1.type_int64_t.store_int64_t(texture_height);
    GLIS_INTERNAL_SHARED_MEMORY.slot.additional_data_2.type_size_t.store_size_t(window_id);
    auto s2 = now_ms();
    glReadPixels(0, 0, texture_width, texture_height, GL_RGBA, GL_UNSIGNED_BYTE, GLIS_INTERNAL_SHARED_MEMORY.slot.texture.load_ptr());
    auto e2 = now_ms();
    LOG_INFO("glReadPixels completed in %ld milliseconds", e2-s2);
    GLIS_error_to_string_GL("glReadPixels");
    GLIS_sync_server("GLIS_upload_texture", window_id);
    LOG_INFO("uploaded texture");
}

void GLIS::GLIS_upload_texture(GLIS_CLASS &GLIS, size_t &window_id,
                               const GLint &texture_width, const GLint &texture_height) {
    GLIS_upload_texture(GLIS, window_id, GLIS_current_texture, texture_width, texture_height);
}

bool GLIS::getAndroidWindow(void * jenv, void * surface, GLIS_CLASS & GLIS, int width, int height) {
#ifndef __ANDROID__
    LOG_ERROR("function not implemented in Linux");
    return false;
#else
    GLIS.native_window = ANativeWindow_fromSurface(static_cast<JNIEnv*>(jenv), static_cast<jobject>(surface));
    if (GLIS.native_window == 0) {
        LOG_ERROR("error, cannot obtain a native window");
        return false;
    }
    GLIS.width = width;
    GLIS.height = height;
    return true;
#endif
}

bool GLIS::runUntilAndroidWindowClose(
        GLIS & glis,
        GLIS_CLASS & glis_class,
        GLIS_FONT & glis_font,
        GLIS_FPS & glis_fps,
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
) {
    while (glis.SYNC_STATE != glis.STATE.request_shutdown) {
        if (onWindowDraw != nullptr)
            onWindowDraw(glis, glis_class, glis_font, glis_fps);
    }
    return true;
}

bool GLIS::destroyAndroidWindow(GLIS_CLASS & GLIS) {
#ifndef __ANDROID__
    LOG_ERROR("function not implemented in Linux");
    return false;
#else
    ANativeWindow_release(GLIS.native_window);
    GLIS.native_window = 0;
    return true;
#endif
}

bool GLIS::getX11Window(GLIS_CLASS & GLIS, int width, int height) {
#ifdef __ANDROID__
    LOG_ERROR("function not implemented in android");
    return false;
#else
    // create a new X11 window
    GLIS.display_id = XOpenDisplay(nullptr);
    if (GLIS.display_id == 0) {
        LOG_ERROR("error, cannot connect to X server");
        return false;
    }
    GLIS.native_window = XCreateSimpleWindow(
            GLIS.display_id, DefaultRootWindow(GLIS.display_id), 0, 0, width, height, 0, 0, 0
    );
    // StructureNotifyMask so we can handle resizing
    XSelectInput(GLIS.display_id, GLIS.native_window, StructureNotifyMask);
    GLIS.width = width;
    GLIS.height = height;
    XMapWindow(GLIS.display_id, GLIS.native_window);
    XStoreName(GLIS.display_id, GLIS.native_window, "Compositor");

    // obtain the current dpi

    char *resourceString = XResourceManagerString(GLIS.display_id);
    XrmDatabase db;
    XrmValue value;
    char *type = NULL;

    XrmInitialize(); /* Need to initialize the DB before calling Xrm* functions */

    db = XrmGetStringDatabase(resourceString);

    if (resourceString) {
        if (XrmGetResource(db, "Xft.dpi", "String", &type, &value) == True) {
            if (value.addr) {
                GLIS.dpi = atoi(value.addr);
            }
        }
    }
    LOG_INFO("DPI: %d", GLIS.dpi);
    return true;
#endif
}

#ifndef __ANDROID__
int predicate (Display *display, XEvent *event, XPointer arg) {
    return true;
}
#endif

bool GLIS::runUntilX11WindowClose(
        GLIS & glis,
        GLIS_CLASS & glis_class,
        GLIS_FONT & glis_font,
        GLIS_FPS & glis_fps,
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
) {
#ifdef __ANDROID__
    LOG_ERROR("function not implemented in android");
    return false;
#else
    Atom wmDeleteMessage = XInternAtom(glis_class.display_id, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(glis_class.display_id, glis_class.native_window, &wmDeleteMessage, 1);
    XEvent event;
    bool running = true;

    while (running) {
        if (onWindowDraw != nullptr)
            onWindowDraw(glis, glis_class, glis_font, glis_fps);
        if (XCheckIfEvent(glis_class.display_id, &event, predicate, nullptr)) {
            if (event.type == ClientMessage) {
                if (event.xclient.data.l[0] == wmDeleteMessage) {
                    if (onWindowClose != nullptr)
                        onWindowClose(glis, glis_class, glis_font, glis_fps);
                    running = false;
                }
            } else if (event.type == ConfigureNotify) {
                glis_class.width = event.xconfigure.width;
                glis_class.height = event.xconfigure.height;
                if (onWindowResize != nullptr)
                    onWindowResize(glis, glis_class, glis_font, glis_fps);
            }
        }
    }
    return true;
#endif
}

bool GLIS::destroyX11Window(GLIS_CLASS & GLIS) {
#ifdef __ANDROID__
    LOG_ERROR("function not implemented in android");
    return false;
#else
    XDestroyWindow(GLIS.display_id, GLIS.native_window);
    XCloseDisplay(GLIS.display_id);
    return true;
#endif
}

#ifndef __ANDROID__
// TODO: move these into GLIS_CLASS
static bool running = true;
struct wl_surface *surface;
struct xdg_surface *xdg_surface;
static struct xdg_wm_base *xdg_wm_base = nullptr;
static struct xdg_toplevel *xdg_toplevel = nullptr;
static struct wl_compositor *compositor = nullptr;

static void noop(void* a, struct xdg_toplevel * b, int32_t c, int32_t d, struct wl_array * e) {
    // This space intentionally left blank
}

static void xdg_surface_handle_configure(void *data,
                                         struct xdg_surface *xdg_surface, uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_handle_configure,
};

static void xdg_toplevel_handle_close(void *data,
                                      struct xdg_toplevel *xdg_toplevel) {
    running = false;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
        .configure = noop,
        .close = xdg_toplevel_handle_close,
};

static void pointer_handle_button(void *data, struct wl_pointer *pointer,
                                  uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    struct wl_seat *seat = reinterpret_cast<struct wl_seat *>(data);

    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        xdg_toplevel_move(xdg_toplevel, seat, serial);
    }
    if (button == BTN_RIGHT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        running = false;
    }
}

static const struct wl_pointer_listener pointer_listener = {
        .enter = reinterpret_cast<void (*)(void*, struct wl_pointer*, uint32_t, struct wl_surface*, wl_fixed_t, wl_fixed_t)>(noop),
        .leave = reinterpret_cast<void (*)(void*, struct wl_pointer*, uint32_t, struct wl_surface*)>(noop),
        .motion = reinterpret_cast<void (*)(void*, struct wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t)>(noop),
        .button = pointer_handle_button,
        .axis = reinterpret_cast<void (*)(void*, struct wl_pointer*, uint32_t, uint32_t, wl_fixed_t)>(noop),
};

static void seat_handle_capabilities(void *data, struct wl_seat *seat,
                                     uint32_t capabilities) {
    if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
        struct wl_pointer *pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, seat);
    }
}

static const struct wl_seat_listener seat_listener = {
        .capabilities = seat_handle_capabilities,
};

static void handle_global(void *data, struct wl_registry *registry,
                          uint32_t name, const char *interface, uint32_t version) {
    if (strcmp(interface, wl_seat_interface.name) == 0) {
        struct wl_seat *seat = reinterpret_cast<struct wl_seat *>(wl_registry_bind(registry, name, &wl_seat_interface, 1));
        wl_seat_add_listener(seat, &seat_listener, nullptr);
    } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
        compositor = reinterpret_cast<wl_compositor *>(wl_registry_bind(registry, name, &wl_compositor_interface, 1));
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        xdg_wm_base = reinterpret_cast<struct xdg_wm_base *>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 1));
    }
}

static void handle_global_remove(void *data, struct wl_registry *registry,
                                 uint32_t name) {
    // Who cares
}

static const struct wl_registry_listener registry_listener = {
        .global = handle_global,
        .global_remove = handle_global_remove,
};
#endif

bool GLIS::getWaylandWindow(GLIS_CLASS & GLIS, int width, int height) {
#ifdef __ANDROID__
    LOG_ERROR("function not implemented in android");
    return false;
#else
    // https://github.com/emersion/hello-wayland/blob/opengl/main.c
    // create a new Wayland window
    GLIS.display_id = reinterpret_cast<EGLNativeDisplayType>(wl_display_connect(nullptr));
    if (GLIS.display_id == nullptr) {
        LOG_ERROR("error, cannot connect to Wayland");
        return false;
    }

    struct wl_registry *registry = wl_display_get_registry(reinterpret_cast<struct wl_display *>(GLIS.display_id));
    wl_registry_add_listener(registry, &registry_listener, nullptr);
    wl_display_dispatch(reinterpret_cast<struct wl_display *>(GLIS.display_id));
    wl_display_roundtrip(reinterpret_cast<struct wl_display *>(GLIS.display_id));
    if (compositor == nullptr || xdg_wm_base == nullptr) {
        LOG_ERROR("no wl_shm, wl_compositor or xdg_wm_base support");
        return false;
    }
    surface = wl_compositor_create_surface(compositor);
    xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    xdg_toplevel_set_min_size(xdg_toplevel, width, height);
    xdg_toplevel_set_max_size(xdg_toplevel, width, height);
    wl_surface_commit(surface);
    wl_display_roundtrip(reinterpret_cast<struct wl_display *>(GLIS.display_id));
    GLIS.native_window = reinterpret_cast<EGLNativeWindowType>(wl_egl_window_create(surface, width, height));
    return true;
#endif
}

bool GLIS::runUntilWaylandWindowClose(
        GLIS & glis,
        GLIS_CLASS & glis_class,
        GLIS_FONT & glis_font,
        GLIS_FPS & glis_fps,
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
) {
#ifdef __ANDROID__
    LOG_ERROR("function not implemented in android");
    return false;
#else
    while(wl_display_dispatch(reinterpret_cast<struct wl_display *>(glis_class.display_id)) != -1 && running) {
        if (onWindowDraw != nullptr)
            onWindowDraw(glis, glis_class, glis_font, glis_fps);
    }
    if (onWindowClose != nullptr)
        onWindowClose(glis, glis_class, glis_font, glis_fps);
    return true;
#endif
}


bool GLIS::destroyWaylandWindow(GLIS_CLASS & GLIS) {
#ifdef __ANDROID__
    LOG_ERROR("function not implemented in android");
    return false;
#else
    xdg_toplevel_destroy(xdg_toplevel);
    xdg_surface_destroy(xdg_surface);
    wl_surface_destroy(surface);
    return true;
#endif
}

void GLIS::GLIS_draw_high_resolution_square() {
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions           // colors           // texture coords
            1.0f,   1.0f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            1.0f,  -1.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void GLIS::GLIS_Viewport(class GLIS_CLASS &GLIS) {
    glViewport(0, 0, GLIS.width, GLIS.height);
}

GLIS::GLIS_shared_memory::slot_::slot_() {}

GLIS::GLIS_shared_memory::slot_::slot__::slot__() {}

GLIS::GLIS_shared_memory::slot_::slot__::slot__(GLIS::GLIS_shared_memory *pMemory) {
    shared_memory = pMemory;
    assert(pMemory != 0x0);
}

void *GLIS::GLIS_shared_memory::slot_::slot__::load_ptr() {
    return &shared_memory->data[slot];
}

void GLIS::GLIS_shared_memory::slot_::slot__::store_int8_t(int8_t value) {
    shared_memory->data[slot] = value;
}

void GLIS::GLIS_shared_memory::slot_::slot__::store_int16_t(int16_t value) {
    reinterpret_cast<int16_t*>(&shared_memory->data[slot])[0] = value;
}

void GLIS::GLIS_shared_memory::slot_::slot__::store_int32_t(int32_t value) {
    reinterpret_cast<int32_t*>(&shared_memory->data[slot])[0] = value;
}

void GLIS::GLIS_shared_memory::slot_::slot__::store_int64_t(int64_t value) {
    reinterpret_cast<int64_t*>(&shared_memory->data[slot])[0] = value;
}

void GLIS::GLIS_shared_memory::slot_::slot__::store_size_t(size_t value) {
    reinterpret_cast<size_t*>(&shared_memory->data[slot])[0] = value;
}

int8_t GLIS::GLIS_shared_memory::slot_::slot__::load_int8_t() {
    return shared_memory->data[slot];
}

int16_t GLIS::GLIS_shared_memory::slot_::slot__::load_int16_t() {
    return reinterpret_cast<int16_t*>(&shared_memory->data[slot])[0];
}

int32_t GLIS::GLIS_shared_memory::slot_::slot__::load_int32_t() {
    return reinterpret_cast<int32_t*>(&shared_memory->data[slot])[0];
}

int64_t GLIS::GLIS_shared_memory::slot_::slot__::load_int64_t() {
    return reinterpret_cast<int64_t*>(&shared_memory->data[slot])[0];
}

size_t GLIS::GLIS_shared_memory::slot_::slot__::load_size_t() {
    return reinterpret_cast<size_t*>(&shared_memory->data[slot])[0];
}

GLIS::GLIS_shared_memory::slot_::multi_size::multi_size() {}

GLIS::GLIS_shared_memory::slot_::multi_size::multi_size(GLIS::GLIS_shared_memory *pMemory) {
    type_int8_t = slot__(pMemory);
    type_int16_t = slot__(pMemory);
    type_int32_t = slot__(pMemory);
    type_int64_t = slot__(pMemory);
    type_size_t = slot__(pMemory);
}

void GLIS::clearBlack() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLIS::clearWhite() {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}
