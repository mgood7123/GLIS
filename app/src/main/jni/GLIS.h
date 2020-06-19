//
// Created by konek on 7/26/2019.
//

#ifndef GLNE_GLIS_H
#define GLNE_GLIS_H

#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES3/gl32.h>
#include <cstdlib>
#include <cassert>
#include <malloc.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <cerrno>
#include <unistd.h>
#include <poll.h>

#include "logger.h"
#include "server.h"
#include "WINAPI/SDK/include/Windows/windows.h"
#include "GLIS_IPC.h"

#define LOG_TAG "EglSample"

bool GLIS_LOG_PRINT_NON_ERRORS = false;
bool GLIS_LOG_PRINT_VERTEX = false;
bool GLIS_LOG_PRINT_CONVERSIONS = false;
bool GLIS_LOG_PRINT_SHAPE_INFO = false;

void GLIS_FORK(const char *__file, char *const *__argv) {
    errno = 0;
    pid_t pid = fork();
    LOG_ERROR("pid: %d", pid);
    if (pid == 0) {
        execvp(__file, __argv);
        LOG_ERROR("Cannot exec(%s) - %s\n", __file, strerror(errno));
        exit(1);
    }
}

std::string GLIS_INTERNAL_MESSAGE_PREFIX = "";

#define GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGING_FUNCTION, CASE_NAME, name, const, constSTRING, UNNAMED_STRING_CAN_PRINT_ERROR, UNNAMED_STRING_CANNOT_PRINT_ERROR, NAMED_STRING_CAN_PRINT_ERROR, NAMED_STRING_CANNOT_PRINT_ERROR, PRINT) CASE_NAME: { \
    if(name == nullptr || name == NULL || name == 0) { \
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
    break; \
}


#define GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOGGER, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGER, case const, name, const, constSTRING, nullptr, UNNAMED_STRING, nullptr, NAMED_STRING, false)
    
#define GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING(LOGGER, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGER, case const, name, const, constSTRING, UNNAMED_STRING, nullptr, NAMED_STRING, nullptr, true)

#define GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING_DONT_PRINT_ERROR(name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOG_ERROR, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING)

#define GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING(name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING(LOG_ERROR, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING)

#define GLIS_ERROR_SWITCH_CASE(name, const) \
    GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING(name, const, #const, "%s", "%s generated error: %s")

#define GLIS_ERROR_SWITCH_CASE_DEFAULT(name, err) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOG_ERROR, default, name, err, err, "Unknown error: %d", "Unknown error", "%s generated an unknown error: %d", "%s generated an unknown error", true)

#define GLIS_boolean_to_string(val, TRUE_VALUE) val == TRUE_VALUE ? "true" : "false"

#define GLIS_SHADER_SOURCE__BEGIN R"glsl(
#define GLIS_SHADER_SOURCE__END )glsl";
#define GLIS_SHADER_SOURCE__(S, source, E) S source E
#define GLIS_SHADER_SOURCE(source) GLIS_SHADER_SOURCE__(GLIS_SHADER_SOURCE__BEGIN, source, GLIS_SHADER_SOURCE__END)

int GLIS_ERROR_PRINTING_TYPE_FORMAL = 1;
int GLIS_ERROR_PRINTING_TYPE_CODE = 2;
int GLIS_ERROR_PRINTING_TYPE = GLIS_ERROR_PRINTING_TYPE_FORMAL;

#define GLIS_error_to_string_exec(x) x; if (GLIS_ERROR_PRINTING_TYPE == GLIS_ERROR_PRINTING_TYPE_FORMAL) { GLIS_error_to_string(#x); } else { LOG_INFO("%s", std::string(std::string(#x) + ";").c_str()); }
#define GLIS_error_to_string_exec_GL(x) x; if (GLIS_ERROR_PRINTING_TYPE == GLIS_ERROR_PRINTING_TYPE_FORMAL) { GLIS_error_to_string_GL(#x); } else { LOG_INFO("%s", std::string(std::string(#x) + ";").c_str()); }
#define GLIS_error_to_string_exec_EGL(x) x; if (GLIS_ERROR_PRINTING_TYPE == GLIS_ERROR_PRINTING_TYPE_FORMAL) { GLIS_error_to_string_EGL(#x); } else { LOG_INFO("%s", std::string(std::string(#x) + ";").c_str()); }

void GLIS_error_to_string_GL(const char * name, GLint err) {
    GLIS_INTERNAL_MESSAGE_PREFIX = "OpenGL:          ";
    switch(err) {
        // GENERATED BY glGetError() ITSELF
        GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOG_INFO, name, GL_NO_ERROR,
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

void GLIS_error_to_string_EGL(const char * name, EGLint err) {
    GLIS_INTERNAL_MESSAGE_PREFIX = "OpenGL ES (EGL): ";
    switch(err) {
        // GENERATED BY eglGetError() ITSELF
        GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOG_INFO, name, EGL_SUCCESS,
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

void GLIS_error_to_string_GL(GLint err) {
    GLIS_error_to_string_GL(nullptr, err);
}

void GLIS_error_to_string_EGL(EGLint err) {
    GLIS_error_to_string_EGL(nullptr, err);
}

void GLIS_error_to_string_GL() {
    GLIS_error_to_string_GL(glGetError());
}

void GLIS_error_to_string_EGL() {
    GLIS_error_to_string_EGL(eglGetError());
}

void GLIS_error_to_string() {
    GLIS_error_to_string_GL();
    GLIS_error_to_string_EGL();
}

void GLIS_error_to_string_GL(const char * name) {
    GLIS_error_to_string_GL(name, glGetError());
}

void GLIS_error_to_string_EGL(const char * name) {
    GLIS_error_to_string_EGL(name, eglGetError());
}

void GLIS_error_to_string(const char * name) {
    GLIS_error_to_string_GL(name);
    GLIS_error_to_string_EGL(name);
}

void GLIS_GL_INFORMATION() {
    const GLubyte * vendor = GLIS_error_to_string_exec_GL(glGetString(GL_VENDOR));
    const GLubyte * renderer = GLIS_error_to_string_exec_GL(glGetString(GL_RENDERER));
    const GLubyte * version = GLIS_error_to_string_exec_GL(glGetString(GL_VERSION));
    const GLubyte * slv = GLIS_error_to_string_exec_GL(glGetString(GL_SHADING_LANGUAGE_VERSION));
    const GLubyte * extentions = GLIS_error_to_string_exec_GL(glGetString(GL_EXTENSIONS));
    LOG_INFO("GL_VENDOR: %s", vendor);
    LOG_INFO("GL_RENDERER: %s", renderer);
    LOG_INFO("GL_VERSION: %s", version);
    LOG_INFO("GL_SHADING_LANGUAGE_VERSION: %s", slv);
    LOG_INFO("GL_EXTENSIONS: %s", extentions);
}

class GLIS_CLASS {
    public:
        int init_GLIS = false;
        bool
            init_eglGetDisplay = false,
            init_eglInitialize = false,
            init_eglChooseConfig = false,
            init_eglCreateWindowSurface = false,
            init_eglCreatePbufferSurface = false,
            init_eglCreateContext = false,
            init_eglMakeCurrent = false;
        const GLint
            * configuration_attributes = nullptr,
            * context_attributes = nullptr,
            * surface_attributes = nullptr;
        EGLint
            eglMajVers = 0,
            eglMinVers = 0,
            number_of_configurations = 0;
        EGLDisplay display = EGL_NO_DISPLAY;
        EGLConfig configuration = 0;
        EGLContext
            context = EGL_NO_CONTEXT,
            shared_context = EGL_NO_CONTEXT;
        EGLSurface surface = EGL_NO_SURFACE;
        ANativeWindow *native_window = nullptr;
        GLint
            width = 0,
            height = 0;
        SOCKET_SERVER server;
        Kernel KERNEL;
};

void GLIS_EGL_INFORMATION(EGLDisplay & DISPLAY) {
    const char * client_apis = GLIS_error_to_string_exec_EGL(eglQueryString(DISPLAY, EGL_CLIENT_APIS));
    const char * vendor = GLIS_error_to_string_exec_EGL(eglQueryString(DISPLAY, EGL_VENDOR));
    const char * version = GLIS_error_to_string_exec_EGL(eglQueryString(DISPLAY, EGL_VERSION));
    const char * extentions = GLIS_error_to_string_exec_EGL(eglQueryString(DISPLAY, EGL_EXTENSIONS));
    LOG_INFO("EGL_CLIENT_APIS: %s", client_apis);
    LOG_INFO("EGL_VENDOR: %s", vendor);
    LOG_INFO("EGL_VERSION: %s", version);
    LOG_INFO("EGL_EXTENSIONS: %s", extentions);
}

void GLIS_destroy_GLIS(class GLIS_CLASS & GLIS) {
    if (!GLIS.init_GLIS) return;

    if (GLIS.init_eglMakeCurrent) {
        GLIS_error_to_string_exec_EGL(eglMakeCurrent(GLIS.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
        GLIS.init_eglMakeCurrent = false;
    }
    if(GLIS.init_eglCreateContext) {
        GLIS_error_to_string_exec_EGL(eglDestroyContext(GLIS.display, GLIS.context));
        GLIS.context = EGL_NO_CONTEXT;
        GLIS.shared_context = EGL_NO_CONTEXT;
        GLIS.init_eglCreateContext = false;
    }
    if (GLIS.init_eglCreateWindowSurface || GLIS.init_eglCreatePbufferSurface) {
        GLIS_error_to_string_exec_EGL(eglDestroySurface(GLIS.display, GLIS.surface));
        GLIS.surface = EGL_NO_SURFACE;
        GLIS.init_eglCreateWindowSurface = false;
        GLIS.init_eglCreatePbufferSurface = false;
    }
    if (GLIS.init_eglChooseConfig) {
        // TODO: figure how to undo init_eglChooseConfig
    }
    if (GLIS.init_eglInitialize) {
        GLIS_error_to_string_exec_EGL(eglTerminate(GLIS.display));
        GLIS.init_eglInitialize = false;
    }
    if (GLIS.init_eglGetDisplay) {
        GLIS.display = EGL_NO_DISPLAY;
        GLIS.init_eglGetDisplay = false;
    }
    GLIS.init_GLIS = false;
}


bool GLIS_initialize_display(class GLIS_CLASS & GLIS) {
    GLIS.display = GLIS_error_to_string_exec_EGL(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    if (GLIS.display == EGL_NO_DISPLAY) return false;
    GLIS.init_eglGetDisplay = true;
    EGLBoolean r = GLIS_error_to_string_exec_EGL(eglInitialize(GLIS.display, &GLIS.eglMajVers, &GLIS.eglMinVers));
    if (r == EGL_FALSE) return false;
    GLIS.init_eglInitialize = true;
    LOG_INFO("EGL initialized with version %d.%d", GLIS.eglMajVers, GLIS.eglMinVers);
    GLIS_EGL_INFORMATION(GLIS.display);
    return true;
}

bool GLIS_initialize_configuration(class GLIS_CLASS & GLIS) {
    EGLBoolean r = GLIS_error_to_string_exec_EGL(eglChooseConfig(GLIS.display, GLIS.configuration_attributes, &GLIS.configuration, 1, &GLIS.number_of_configurations));
    if (r == EGL_FALSE) return false;
    GLIS.init_eglChooseConfig = true;
    return true;
}

bool GLIS_initialize_surface_CreateWindowSurface(class GLIS_CLASS & GLIS) {
    GLIS.surface = GLIS_error_to_string_exec_EGL(eglCreateWindowSurface(GLIS.display, GLIS.configuration, GLIS.native_window, nullptr));
    if (GLIS.surface == EGL_NO_SURFACE) return false;
    GLIS.init_eglCreateWindowSurface = true;
    return true;
}

bool GLIS_initialize_surface_CreatePbufferSurface(class GLIS_CLASS & GLIS) {
    GLIS.surface = GLIS_error_to_string_exec_EGL(eglCreatePbufferSurface(GLIS.display, GLIS.configuration, GLIS.surface_attributes));
    if (GLIS.surface == EGL_NO_SURFACE) return false;
    GLIS.init_eglCreatePbufferSurface = true;
    return true;
}

bool GLIS_create_context(class GLIS_CLASS & GLIS) {
    GLIS.context = GLIS_error_to_string_exec_EGL(eglCreateContext(GLIS.display, GLIS.configuration, GLIS.shared_context, GLIS.context_attributes));
    if (GLIS.context == EGL_NO_CONTEXT) return false;
    GLIS.init_eglCreateContext = true;
    return true;
}

bool GLIS_switch_to_context(class GLIS_CLASS & GLIS) {
    EGLBoolean r = GLIS_error_to_string_exec_EGL(eglMakeCurrent(GLIS.display, GLIS.surface, GLIS.surface, GLIS.context));
    if (r == EGL_FALSE) return false;
    GLIS.init_eglMakeCurrent = true;
    GLIS_GL_INFORMATION();
    return true;
}

bool GLIS_get_width_height(class GLIS_CLASS & GLIS) {
    EGLBoolean r1 = GLIS_error_to_string_exec_EGL(eglQuerySurface(GLIS.display, GLIS.surface, EGL_WIDTH, &GLIS.width));
    if (r1 == EGL_FALSE) return false;
    EGLBoolean r2 = GLIS_error_to_string_exec_EGL(eglQuerySurface(GLIS.display, GLIS.surface, EGL_HEIGHT, &GLIS.height));
    if (r2 == EGL_FALSE) return false;
    return true;
}

bool GLIS_initialize(class GLIS_CLASS & GLIS, GLint surface_type) {
    if (GLIS.init_GLIS) return true;

    LOG_INFO("Initializing");

    if ( eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) return false;
    
    LOG_INFO("Initializing display");
    if (!GLIS_initialize_display(GLIS)) {
        LOG_ERROR("Failed to initialize display");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Initialized display");
    LOG_INFO("Initializing configuration");
    if (!GLIS_initialize_configuration(GLIS)) {
        LOG_ERROR("Failed to initialize configuration");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Initialized configuration");
    LOG_INFO("Initializing surface");
    if (surface_type == EGL_WINDOW_BIT) {
        LOG_INFO("Creating window surface");
        if (!GLIS_initialize_surface_CreateWindowSurface(GLIS)) {
            LOG_ERROR("Failed to initialize surface");
            GLIS_destroy_GLIS(GLIS);
            return false;
        }
    }
    else if (surface_type == EGL_PBUFFER_BIT) {
        LOG_INFO("creating pixel buffer surface");
        if (!GLIS_initialize_surface_CreatePbufferSurface(GLIS)) {
            LOG_ERROR("Failed to initialize surface");
            GLIS_destroy_GLIS(GLIS);
            return false;
        }
    }
    LOG_INFO("Initialized surface");
    LOG_INFO("Initializing context");
    if (!GLIS_create_context(GLIS)) {
        LOG_ERROR("Failed to initialize context");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Initialized context");
    LOG_INFO("Switching to context");
    if (!GLIS_switch_to_context(GLIS)) {
        LOG_ERROR("Failed to switch to context");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Switched to context");
    LOG_INFO("Obtaining surface width and height");
    if (!GLIS_get_width_height(GLIS)) {
        LOG_ERROR("Failed to obtain surface width and height");
        GLIS_destroy_GLIS(GLIS);
        return false;
    }
    LOG_INFO("Obtained surface width and height");
    GLIS.init_GLIS = true;
    LOG_INFO("Initialized");
    return true;
}

bool GLIS_setupOnScreenRendering(class GLIS_CLASS & GLIS, EGLContext shared_context) {
    GLIS.shared_context = shared_context;

    const EGLint config[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_NONE };
    GLIS.configuration_attributes = config;

    const EGLint context[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    GLIS.context_attributes = context;

    const EGLint surface[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_NONE };
    GLIS.surface_attributes = surface;

    return GLIS_initialize(GLIS, EGL_WINDOW_BIT);
}

bool GLIS_setupOnScreenRendering(class GLIS_CLASS & GLIS) {
    return GLIS_setupOnScreenRendering(GLIS, EGL_NO_CONTEXT);
}

bool GLIS_INIT_SHARED_MEMORY();
bool GLIS_setupOffScreenRendering(class GLIS_CLASS & GLIS, int w, int h, EGLContext shared_context) {
    if (IPC == IPC_MODE.shared_memory) if (!GLIS_INIT_SHARED_MEMORY()) return false;

    GLIS.shared_context = shared_context;

    const EGLint config[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_NONE };
    GLIS.configuration_attributes = config;

    const EGLint context[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    GLIS.context_attributes = context;

    const EGLint surface[] = { EGL_WIDTH, w, EGL_HEIGHT, h, EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB, EGL_TEXTURE_TARGET, EGL_TEXTURE_2D, EGL_NONE };
    GLIS.surface_attributes = surface;

    return GLIS_initialize(GLIS, EGL_PBUFFER_BIT);
}

bool GLIS_setupOffScreenRendering(class GLIS_CLASS & GLIS, int w, int h) {
    return GLIS_setupOffScreenRendering(GLIS, w, h, EGL_NO_CONTEXT);
}

GLboolean GLIS_ShaderCompilerSupported() {
    GLboolean GLSC_supported;
    GLIS_error_to_string_exec_GL(glGetBooleanv(GL_SHADER_COMPILER, &GLSC_supported));
    LOG_INFO("Supports Shader Compiler: %s", GLSC_supported == GL_TRUE ? "true" : "false");
    return GLSC_supported;
}

GLuint GLIS_createShader(GLenum shaderType, const char * & src) {
    if (GLIS_ShaderCompilerSupported()) {
        const char * SHADER_TYPE = nullptr;
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
        GLuint shader = GLIS_error_to_string_exec_GL(glCreateShader(shaderType));
        if (!shader) {
            return 0;
        }
        GLIS_error_to_string_exec_GL(glShaderSource(shader, 1, &src, nullptr));
        GLint compiled = GL_FALSE;
        LOG_INFO("Compiling %s Shader", SHADER_TYPE);
        GLIS_error_to_string_exec_GL(glCompileShader(shader));
        GLIS_error_to_string_exec_GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
        if (compiled != GL_TRUE) {
            GLint infoLogLen = 0;
            GLIS_error_to_string_exec_GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen));
            if (infoLogLen > 0) {
                GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
                if (infoLog) {
                    GLIS_error_to_string_exec_GL(
                            glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog));
                    LOG_ERROR("Could not compile %s shader:\n%s\n", SHADER_TYPE, infoLog);
                    free(infoLog);
                }
            }
            GLIS_error_to_string_exec_GL(glDeleteShader(shader));
            return 0;
        }
        assert(glIsShader(shader) == GL_TRUE);
        return shader;
    } else return 0;
}

GLboolean GLIS_validate_program_link(GLuint & Program) {
    GLint linked = GL_FALSE;
    GLIS_error_to_string_exec_GL(glGetProgramiv(Program, GL_LINK_STATUS, &linked));
    if (linked != GL_TRUE) {
        GLint infoLogLen = 0;
        GLIS_error_to_string_exec_GL(glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLen));
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {
                GLIS_error_to_string_exec_GL(
                        glGetProgramInfoLog(Program, infoLogLen, nullptr, infoLog));
                LOG_ERROR("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        GLIS_error_to_string_exec_GL(glDeleteProgram(Program));
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean GLIS_validate_program_valid(GLuint & Program) {
    GLint validated = GL_FALSE;
    GLIS_error_to_string_exec_GL(glValidateProgram(Program));
    GLIS_error_to_string_exec_GL(glGetProgramiv(Program, GL_VALIDATE_STATUS, &validated));
    if (validated != GL_TRUE) {
        GLint infoLogLen = 0;
        GLIS_error_to_string_exec_GL(glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLen));
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {
                GLIS_error_to_string_exec_GL(
                        glGetProgramInfoLog(Program, infoLogLen, nullptr, infoLog));
                LOG_ERROR("Could not validate program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        GLIS_error_to_string_exec_GL(glDeleteProgram(Program));
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean GLIS_validate_program(GLuint & Program) {
    if (GLIS_validate_program_link(Program) == GL_TRUE)
        if (GLIS_validate_program_valid(Program) == GL_TRUE) {
            GLboolean v = GLIS_error_to_string_exec_GL(glIsProgram(Program));
            return v;
        }
    return GL_FALSE;
}

/*
// Normalized Device Coordinates (NDC)
                   height
( -1, 1)             | (  0,  1)  (  1,  1)
                     |
                     |
                     |
                     |
                     |
                     |
                     |
                     |
( -1,  0)            | (  0,  0)  (  1,  0)
---------------------+--------------------- width
                     |
                     |
                     |
                     |
                     |
                     |
                     |
                     |
                     |
( -1, -1)            | (  0, -1)   (  1, -1)

// height = 40
// width = 20

GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_TOP_LEFT);
GLIS_convertPair(0,0,20,40);
GLIS_convertPair(0,40,20,40);
GLIS_convertPair(20,0,20,40);
GLIS_convertPair(10,10,20,40);
GLIS_convertPair(10,20,20,40);
GLIS_convertPair(20,20,20,40);
GLIS_convertPair(20,40,20,40);
GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_TOP_RIGHT);
GLIS_convertPair(0,0,20,40);
GLIS_convertPair(0,40,20,40);
GLIS_convertPair(20,0,20,40);
GLIS_convertPair(10,10,20,40);
GLIS_convertPair(10,20,20,40);
GLIS_convertPair(20,20,20,40);
GLIS_convertPair(20,40,20,40);
GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT);
GLIS_convertPair(0,0,20,40);
GLIS_convertPair(0,40,20,40);
GLIS_convertPair(20,0,20,40);
GLIS_convertPair(10,10,20,40);
GLIS_convertPair(10,20,20,40);
GLIS_convertPair(20,20,20,40);
GLIS_convertPair(20,40,20,40);
GLIS_set_conversion_origin(GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT);
GLIS_convertPair(0,0,20,40);
GLIS_convertPair(0,40,20,40);
GLIS_convertPair(20,0,20,40);
GLIS_convertPair(10,10,20,40);
GLIS_convertPair(10,20,20,40);
GLIS_convertPair(20,20,20,40);
GLIS_convertPair(20,40,20,40);

    converting with origin top left
    inverting 'y'
    width: 0, height: 0, ConvertPair: -1.000000, 1.000000
    inverting 'y'
    width: 0, height: 40, ConvertPair: -1.000000, -1.000000
    inverting 'y'
    width: 20, height: 0, ConvertPair: 1.000000, 1.000000
    inverting 'y'
    width: 10, height: 10, ConvertPair: 0.000000, 0.500000
    inverting 'y'
    width: 10, height: 20, ConvertPair: 0.000000, -0.000000
    inverting 'y'
    width: 20, height: 20, ConvertPair: 1.000000, -0.000000
    inverting 'y'
    width: 20, height: 40, ConvertPair: 1.000000, -1.000000
    converting with origin top right
    inverting 'x'
    inverting 'y'
    width: 0, height: 0, ConvertPair: 1.000000, 1.000000
    inverting 'x'
    inverting 'y'
    width: 0, height: 40, ConvertPair: 1.000000, -1.000000
    inverting 'x'
    inverting 'y'
    width: 20, height: 0, ConvertPair: -1.000000, 1.000000
    inverting 'x'
    inverting 'y'
    width: 10, height: 10, ConvertPair: -0.000000, 0.500000
    inverting 'x'
    inverting 'y'
    width: 10, height: 20, ConvertPair: -0.000000, -0.000000
    inverting 'x'
    inverting 'y'
    width: 20, height: 20, ConvertPair: -1.000000, -0.000000
    inverting 'x'
    inverting 'y'
    width: 20, height: 40, ConvertPair: -1.000000, -1.000000
    converting with origin bottom left
    no conversion
    width: 0, height: 0, ConvertPair: -1.000000, -1.000000
    no conversion
    width: 0, height: 40, ConvertPair: -1.000000, 1.000000
    no conversion
    width: 20, height: 0, ConvertPair: 1.000000, -1.000000
    no conversion
    width: 10, height: 10, ConvertPair: 0.000000, -0.500000
    no conversion
    width: 10, height: 20, ConvertPair: 0.000000, 0.000000
    no conversion
    width: 20, height: 20, ConvertPair: 1.000000, 0.000000
    no conversion
    width: 20, height: 40, ConvertPair: 1.000000, 1.000000
    converting with origin bottom right
    inverting 'x'
    width: 0, height: 0, ConvertPair: 1.000000, -1.000000
    inverting 'x'
    width: 0, height: 40, ConvertPair: 1.000000, 1.000000
    inverting 'x'
    width: 20, height: 0, ConvertPair: -1.000000, -1.000000
    inverting 'x'
    width: 10, height: 10, ConvertPair: -0.000000, -0.500000
    inverting 'x'
    width: 10, height: 20, ConvertPair: -0.000000, 0.000000
    inverting 'x'
    width: 20, height: 20, ConvertPair: -1.000000, 0.000000
    inverting 'x'
    width: 20, height: 40, ConvertPair: -1.000000, 1.000000
*/

template <typename TYPE>
float GLIS_inverse(TYPE num) {
    return num<0?-num:-(num);
}

template <typename TYPE>
float GLIS_convert(TYPE num, TYPE num_max) {
    // 3 year old magic
    return (num-(num_max/2))/(num_max/2);
}

template <typename TYPE>
class GLIS_coordinates {
    public:
        GLIS_coordinates(TYPE TYPE_INITIALIZER) {
            TYPE x = TYPE_INITIALIZER;
            TYPE y = TYPE_INITIALIZER;
        }

        TYPE x;
        TYPE y;
};

template <typename TYPE>
class GLIS_rect {
    public:
        GLIS_rect(TYPE TYPE_INITIALIZER) {
            topLeft = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
            topRight = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
            bottomLeft = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
            bottomRight = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
        }

        class GLIS_coordinates <TYPE> topLeft = NULL;
        class GLIS_coordinates <TYPE> topRight = NULL;
        class GLIS_coordinates <TYPE> bottomLeft = NULL;
        class GLIS_coordinates <TYPE> bottomRight = NULL;
};

template <typename TYPE>
class GLIS_rect<TYPE> GLIS_points_to_rect(TYPE TYPE_INITIALIZER, TYPE x1, TYPE y1, TYPE x2, TYPE y2) {
    class GLIS_rect<TYPE> r(TYPE_INITIALIZER);
    r.bottomLeft.x = x1;
    r.bottomLeft.y = y1;
    r.topRight.x = x2;
    r.topRight.y = y2;
    r.bottomRight.x = x2;
    r.bottomRight.y = y1;
    r.topLeft.x = x1;
    r.topLeft.y = y2;
    return r;
}

#define GLIS_CONVERSION_ORIGIN_TOP_LEFT 0
#define GLIS_CONVERSION_ORIGIN_TOP_RIGHT 1
#define GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT 2
#define GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT 3

int GLIS_CONVERSION_ORIGIN = GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT;

void GLIS_set_conversion_origin(int origin) {
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

template <typename TYPEFROM, typename TYPETO>
class GLIS_coordinates<TYPETO> GLIS_convertPair(TYPETO TYPETO_INITIALIZER, TYPEFROM x, TYPEFROM y, TYPEFROM x_max, TYPEFROM y_max, bool clip) {
    class GLIS_coordinates<TYPETO> xy(TYPETO_INITIALIZER);
    if (x > x_max) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("x is out of bounds (expected %hi, got %hi)", x_max, x);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", x_max);
            x = x_max;
        }
    }
    else if (x < 0) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("x is out of bounds (expected %hi, got %hi)\n", 0, x);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", 0);
            x = 0;
        }
    }
    if (y > y_max) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("y is out of bounds (expected %hi, got %hi)\n", y_max, y);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", y_max);
            y = y_max;
        }
    }
    else if (y < 0) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("y is out of bounds (expected %hi, got %hi)\n", 0, y);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", 0);
            y = 0;
        }
    }
    xy.x = GLIS_convert<TYPETO>(static_cast<TYPETO>(x), static_cast<TYPETO>(x_max)); // x
    xy.y = GLIS_convert<TYPETO>(static_cast<TYPETO>(y), static_cast<TYPETO>(y_max)); // y
    switch (GLIS_CONVERSION_ORIGIN) {
        case GLIS_CONVERSION_ORIGIN_TOP_LEFT:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'y'");
            xy.y = GLIS_inverse<TYPETO>(xy.y);
            break;
        case GLIS_CONVERSION_ORIGIN_TOP_RIGHT:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'x'");
            xy.x = GLIS_inverse<TYPETO>(xy.x);
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'y'");
            xy.y = GLIS_inverse<TYPETO>(xy.y);
            break;
        case GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT: {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("no conversion");
            break;
        }
        case GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'x'");
            xy.x = GLIS_inverse<TYPETO>(xy.x);
            break;
        default:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("unknown conversion");
            break;
    }
    if (GLIS_LOG_PRINT_CONVERSIONS)
        LOG_INFO("width: %hi, width_max: %hi, height: %hi, height_max: %hi, ConvertPair: %f, %f\n",
                 x, x_max, y, y_max, xy.x, xy.y);
    return xy;
}

template <typename TYPEFROM, typename TYPETO>
class GLIS_coordinates<TYPETO> GLIS_convertPair(TYPETO TYPETO_INITIALIZER, TYPEFROM x, TYPEFROM y, TYPEFROM x_max, TYPEFROM y_max) {
    return GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER, x, y, x_max, y_max, true);
}

template <typename TYPE>
struct GLIS_quater_position {
    TYPE x;
    TYPE y;
    TYPE z;
};

template <typename TYPE>
struct GLIS_quater_color {
    TYPE R;
    TYPE G;
    TYPE B;
};

template <typename TYPE>
struct GLIS_quater {
    struct GLIS_quater_position<TYPE> position;
    struct GLIS_quater_color<TYPE> color;
    struct GLIS_quater_position<TYPE> texture_position;
};

template <typename TYPE>
struct GLIS_vertex_map_rectangle {
    struct GLIS_quater<TYPE> top_right;
    struct GLIS_quater<TYPE> bottom_right;
    struct GLIS_quater<TYPE> bottom_left;
    struct GLIS_quater<TYPE> top_left;
};

template <typename TYPE>
class GLIS_vertex_data {
    public:
        TYPE *vertex;
        int size_of_position;
        int size_of_color;
        int size_of_texture_position;
        int size_per_quater;
        int number_of_points;
        size_t vertex_size;
        unsigned int *indices;
        size_t indices_size;
        size_t typesize;

        void print(const char *format) {
            std::string fmt = "\n";
            fmt += "VECTOR OUTPUT:    |       positions       |       colors          |texture positions|";
            fmt += "\n";
            fmt += "VALUES:           |   X      Y      Z     |   R      B      G     |   X      Y      |";
            fmt += "\n";
            fmt += "TOP RIGHT:      ";
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format;
            fmt += "   |";
            fmt += "\n";
            fmt += "BOTTOM RIGHT:   ";
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format;
            fmt += "   |";
            fmt += "\n";
            fmt += "BOTTOM LEFT:    ";
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format;
            fmt += "   |";
            fmt += "\n";
            fmt += "TOP LEFT:       ";
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format; fmt += ", "; fmt += format;
            fmt += "  |  ";
            fmt += format; fmt += ", "; fmt += format;
            fmt += "   |";
            LOG_INFO(fmt.c_str(),
                     vertex[0], vertex[1], vertex[2],
                     vertex[3], vertex[4], vertex[5],
                     vertex[6], vertex[7],
                     vertex[8], vertex[9], vertex[10],
                     vertex[11], vertex[12], vertex[13],
                     vertex[14], vertex[15],
                     vertex[16], vertex[17], vertex[18],
                     vertex[19], vertex[20], vertex[21],
                     vertex[22], vertex[23],
                     vertex[24], vertex[25], vertex[26],
                     vertex[27], vertex[28], vertex[29],
                     vertex[30], vertex[31]
            );
        }

        void init_attributes() {
            // position attribute
            GLIS_error_to_string_exec_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
            GLIS_error_to_string_exec_GL(glEnableVertexAttribArray(0));
            // color attribute
            GLIS_error_to_string_exec_GL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
            GLIS_error_to_string_exec_GL(glEnableVertexAttribArray(1));
            // texture coord attribute
            GLIS_error_to_string_exec_GL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
            GLIS_error_to_string_exec_GL(glEnableVertexAttribArray(2));
        }
};

template <typename TYPE>
void GLIS_fill_vertex_rect(TYPE * vertex, struct GLIS_quater<TYPE> & quater, int offset) {
    vertex[offset+0] = quater.position.x;
    vertex[offset+1] = quater.position.y;
    vertex[offset+2] = quater.position.z;
    vertex[offset+3] = quater.color.R;
    vertex[offset+4] = quater.color.G;
    vertex[offset+5] = quater.color.B;
    vertex[offset+6] = quater.texture_position.x;
    vertex[offset+7] = quater.texture_position.y;
};

template <typename TYPE>
struct GLIS_vertex_data<TYPE> GLIS_build_vertex_rect(struct GLIS_vertex_map_rectangle<TYPE> & data) {
    struct GLIS_vertex_data<TYPE> v;
    v.typesize = sizeof(TYPE);
    v.size_of_position = 3;
    v.size_of_color = 3;
    v.size_of_texture_position = 2;
    v.size_per_quater = v.size_of_position+v.size_of_color+v.size_of_texture_position;
    v.number_of_points = 4;
    v.vertex_size = v.typesize*(v.size_per_quater*v.number_of_points);
    v.vertex = static_cast<TYPE*>(malloc(v.vertex_size));
    GLIS_fill_vertex_rect(v.vertex, data.top_right, 0*v.size_per_quater);
    GLIS_fill_vertex_rect(v.vertex, data.bottom_right, 1*v.size_per_quater);
    GLIS_fill_vertex_rect(v.vertex, data.bottom_left, 2*v.size_per_quater);
    GLIS_fill_vertex_rect(v.vertex, data.top_left, 3*v.size_per_quater);
    v.indices_size = 6 * sizeof(unsigned int);
    v.indices = static_cast<unsigned int *>(malloc(v.indices_size));
    v.indices[0] = 0;
    v.indices[1] = 1;
    v.indices[2] = 3;
    v.indices[3] = 1;
    v.indices[4] = 2;
    v.indices[5] = 3;
    return v;
}

template <typename TYPEFROM, typename TYPETO>
struct GLIS_vertex_map_rectangle<TYPETO> GLIS_build_vertex_data_rect(TYPETO TYPETO_INITIALIZER, class GLIS_rect<TYPEFROM> data, TYPEFROM max_x, TYPEFROM max_y) {
    struct GLIS_vertex_map_rectangle<TYPETO> m;
    GLIS_coordinates<TYPETO> point1 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER, data.topLeft.x,data.topLeft.y,max_x,max_y);
    m.top_left.position.x = point1.x;
    m.top_left.position.y = point1.y;
    m.top_left.position.z = 0.0F;
    m.top_left.texture_position.x = 0.0F;
    m.top_left.texture_position.y = 1.0F;
    m.top_left.texture_position.z = 0.0F;
    m.top_left.color.R = 1.0F;
    m.top_left.color.G = 1.0F;
    m.top_left.color.B = 1.0F;
    GLIS_coordinates<TYPETO> point2 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER, data.topRight.x,data.topRight.y,max_x,max_y);
    m.top_right.position.x = point2.x;
    m.top_right.position.y = point2.y;
    m.top_right.position.z = 0.0F;
    m.top_right.texture_position.x = 1.0F;
    m.top_right.texture_position.y = 1.0F;
    m.top_right.texture_position.z = 0.0F;
    m.top_right.color.R = 0.0F;
    m.top_right.color.G = 1.0F;
    m.top_right.color.B = 0.0F;
    GLIS_coordinates<TYPETO> point3 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER, data.bottomLeft.x,data.bottomLeft.y,max_x,max_y);
    m.bottom_left.position.x = point3.x;
    m.bottom_left.position.y = point3.y;
    m.bottom_left.position.z = 0.0F;
    m.bottom_left.texture_position.x = 0.0F;
    m.bottom_left.texture_position.y = 0.0F;
    m.bottom_left.texture_position.z = 0.0F;
    m.bottom_left.color.R = 1.0F;
    m.bottom_left.color.G = 0.0F;
    m.bottom_left.color.B = 0.0F;
    GLIS_coordinates<TYPETO> point4 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER, data.bottomRight.x,data.bottomRight.y,max_x,max_y);
    m.bottom_right.position.x = point4.x;
    m.bottom_right.position.y = point4.y;
    m.bottom_right.position.z = 0.0F;
    m.bottom_right.texture_position.x = 1.0F;
    m.bottom_right.texture_position.y = 0.0F;
    m.bottom_right.texture_position.z = 0.0F;
    m.bottom_right.color.R = 0.0F;
    m.bottom_right.color.G = 0.0F;
    m.bottom_right.color.B = 1.0F;
    return m;
}

class GLIS_BACKUP {
    public:
        struct {
            GLint __GL_READ_FRAMEBUFFER_BINDING, __GL_DRAW_FRAMEBUFFER_BINDING;
        } framebuffer;
        struct {
            GLint __GL_RENDERBUFFER_BINDING, __GL_RENDERBUFFER_WIDTH, __GL_RENDERBUFFER_HEIGHT,
                __GL_RENDERBUFFER_INTERNAL_FORMAT, __GL_RENDERBUFFER_RED_SIZE,
                __GL_RENDERBUFFER_GREEN_SIZE, __GL_RENDERBUFFER_BLUE_SIZE,
                __GL_RENDERBUFFER_ALPHA_SIZE, __GL_RENDERBUFFER_DEPTH_SIZE,
                __GL_RENDERBUFFER_STENCIL_SIZE, __GL_RENDERBUFFER_SAMPLES;
        } renderbuffer;
        struct {
            GLint __GL_ACTIVE_TEXTURE, __GL_TEXTURE_BUFFER_BINDING, __GL_VERTEX_ARRAY_BINDING,
                __GL_ARRAY_BUFFER_BINDING, __GL_ELEMENT_ARRAY_BUFFER_BINDING;
        } texture;
        struct {
            GLint __GL_CURRENT_PROGRAM;
        } program;
};

void GLIS_backup_framebuffer(GLIS_BACKUP &backup) {
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,
                                               &backup.framebuffer.__GL_READ_FRAMEBUFFER_BINDING));
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,
                                               &backup.framebuffer.__GL_DRAW_FRAMEBUFFER_BINDING));
//    GLIS_error_to_string_exec_GL(glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_BACK, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE, backup.framebuffer.__GL_READ_FRAMEBUFFER_BINDING));
}

void GLIS_backup_renderbuffer(GLIS_BACKUP &backup) {
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &backup.renderbuffer.__GL_RENDERBUFFER_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_WIDTH));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_HEIGHT));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_INTERNAL_FORMAT));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_RED_SIZE));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_GREEN_SIZE));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_BLUE_SIZE));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_ALPHA_SIZE));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_DEPTH_SIZE));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_STENCIL_SIZE));
    GLIS_error_to_string_exec_GL(
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES,
                                     &backup.renderbuffer.__GL_RENDERBUFFER_SAMPLES));
}

void GLIS_backup_texture(GLIS_BACKUP &backup) {
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_ACTIVE_TEXTURE, &backup.texture.__GL_ACTIVE_TEXTURE));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_TEXTURE_BUFFER_BINDING, &backup.texture.__GL_TEXTURE_BUFFER_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &backup.texture.__GL_VERTEX_ARRAY_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &backup.texture.__GL_ARRAY_BUFFER_BINDING));
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,
                                               &backup.texture.__GL_ELEMENT_ARRAY_BUFFER_BINDING));
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,
                                               &backup.texture.__GL_ELEMENT_ARRAY_BUFFER_BINDING));
}

void GLIS_backup_program(GLIS_BACKUP &backup) {
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_CURRENT_PROGRAM, &backup.program.__GL_CURRENT_PROGRAM));
}

void GLIS_backup(GLIS_BACKUP &backup) {
    GLIS_backup_framebuffer(backup);
    GLIS_backup_renderbuffer(backup);
    GLIS_backup_texture(backup);
    GLIS_backup_program(backup);
};

template <typename TYPE>
void GLIS_draw_rectangle(TYPE INITIALIZER, TYPE x1, TYPE y1, TYPE x2, TYPE y2, TYPE max_x, TYPE max_y) {
    class GLIS_rect<GLint> r = GLIS_points_to_rect<GLint>(INITIALIZER, x1, y1, x2, y2);
    struct GLIS_vertex_map_rectangle<float> vmr = GLIS_build_vertex_data_rect<GLint, float>(0.0F, r, max_x, max_y);
    class GLIS_vertex_data<float> v = GLIS_build_vertex_rect<float>(vmr);
    if (GLIS_LOG_PRINT_VERTEX) v.print("%4.1ff");

    GLuint vertex_array_object;
    GLuint vertex_buffer_object;
    GLuint element_buffer_object;
    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Generating buffers");
    GLIS_error_to_string_exec_GL(glGenVertexArrays(1, &vertex_array_object));
    GLIS_error_to_string_exec_GL(glGenBuffers(1, &vertex_buffer_object));
    GLIS_error_to_string_exec_GL(glGenBuffers(1, &element_buffer_object));
    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Binding buffers");
    GLIS_error_to_string_exec_GL(glBindVertexArray(vertex_array_object));
    GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object));
    GLIS_error_to_string_exec_GL(glBufferData(GL_ARRAY_BUFFER, v.vertex_size, v.vertex, GL_STATIC_DRAW));
    GLIS_error_to_string_exec_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object));
    GLIS_error_to_string_exec_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, v.indices_size, v.indices, GL_STATIC_DRAW));
    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Initializing Attributes");
    v.init_attributes();

    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Drawing rectangle");
    GLIS_error_to_string_exec_GL(glBindVertexArray(vertex_array_object));
    GLIS_error_to_string_exec_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
    GLIS_error_to_string_exec_GL(glBindVertexArray(0));
    GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLIS_error_to_string_exec_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GLIS_error_to_string_exec_GL(glDeleteVertexArrays(1, &vertex_array_object));
    GLIS_error_to_string_exec_GL(glDeleteBuffers(1,&vertex_buffer_object));
    GLIS_error_to_string_exec_GL(glDeleteBuffers(1, &element_buffer_object));
}

void GLIS_set_texture(GLenum textureUnit, GLuint texture) {
    GLIS_error_to_string_exec_GL(glActiveTexture(textureUnit));
    GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, texture));
}

template <typename TYPE>
void GLIS_draw_rectangle(GLenum textureUnit, GLuint texture, TYPE INITIALIZER, TYPE x1, TYPE y1, TYPE x2, TYPE y2, TYPE max_x, TYPE max_y) {
    GLIS_set_texture(textureUnit, texture);
    GLIS_draw_rectangle<TYPE>(INITIALIZER, x1, y1, x2, y2, max_x, max_y);
}

void GLIS_texture_buffer(GLuint & framebuffer, GLuint & renderbuffer, GLuint & renderedTexture, GLint & texture_width, GLint & texture_height) {
    GLIS_error_to_string_exec_GL(glGenFramebuffers(1, &framebuffer));
    GLIS_error_to_string_exec_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    GLIS_error_to_string_exec_GL(glGenRenderbuffers(1, &renderbuffer));
    GLIS_error_to_string_exec_GL(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer));
    GLIS_error_to_string_exec_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8UI, texture_width, texture_height));
    GLIS_error_to_string_exec_GL(
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                                  renderbuffer));
    GLIS_error_to_string_exec_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

    GLenum FramebufferStatus = GLIS_error_to_string_exec_GL(
        glCheckFramebufferStatus(GL_FRAMEBUFFER));

    if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("framebuffer is not complete");
    else
        LOG_INFO("framebuffer is complete");

    // create a new texture
    GLIS_error_to_string_exec_GL(glGenTextures(1, &renderedTexture));
    GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, renderedTexture));
    GLIS_error_to_string_exec_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,0));
    GLIS_error_to_string_exec_GL(
        glGenerateMipmap(GL_TEXTURE_2D)); // this DOES NOT affect the total size of read pixels
    GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    // Set "renderedTexture" as our colour attachement #0
    GLIS_error_to_string_exec_GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0));
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    GLIS_error_to_string_exec_GL(glDrawBuffers(1, DrawBuffers)); // "1" is the size of DrawBuffers
}

GLuint GLIS_current_texture = 0;

void GLIS_Sync_GPU() {
//    LOG_INFO("synchronizing with GPU");
    double start = now_ms();
    GLsync GPU = GLIS_error_to_string_exec_GL(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0));
    if (GPU == nullptr) LOG_ERROR("glFenceSync failed");
//    LOG_INFO("synchronizing");
    GLIS_error_to_string_exec_GL(glWaitSync(GPU, 0, GL_TIMEOUT_IGNORED));
//    LOG_INFO("synchronized");
    glDeleteSync(GPU);
    double end = now_ms();
    LOG_INFO("synchronized with GPU in %G milliseconds", end - start);
}

class STATE {
    public:
        int no_state = -1;
        int initialized = 0;
        int request_startup = 1;
        int response_starting_up = 2;
        int response_started_up = 3;
        int request_shutdown = 4;
        int response_shutting_down = 5;
        int response_shutdown = 6;
        int request_upload = 7;
        int response_uploading = 8;
        int response_uploaded = 9;
        int request_render = 10;
        int response_rendering = 11;
        int response_rendered = 12;
} STATE;

int SYNC_STATE = STATE.no_state;

GLuint *TEXDATA = nullptr;
size_t TEXDATA_LEN = 0;

void
GLIS_resize(GLuint **TEXDATA, size_t &TEXDATA_LEN, int width_from, int height_from, int width_to,
            int height_to) {
    GLIS_BACKUP backup;
    // save
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,
                                               &backup.framebuffer.__GL_DRAW_FRAMEBUFFER_BINDING));
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,
                                               &backup.framebuffer.__GL_READ_FRAMEBUFFER_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &backup.renderbuffer.__GL_RENDERBUFFER_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_ACTIVE_TEXTURE, &backup.texture.__GL_ACTIVE_TEXTURE));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_TEXTURE_BUFFER_BINDING, &backup.texture.__GL_TEXTURE_BUFFER_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &backup.texture.__GL_VERTEX_ARRAY_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &backup.texture.__GL_ARRAY_BUFFER_BINDING));
    GLIS_error_to_string_exec_GL(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,
                                               &backup.texture.__GL_ELEMENT_ARRAY_BUFFER_BINDING));
    GLIS_error_to_string_exec_GL(
        glGetIntegerv(GL_CURRENT_PROGRAM, &backup.program.__GL_CURRENT_PROGRAM));
    const char *CHILDvertexSource = R"glsl( #version 320 es
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

    const char *CHILDfragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;
in highp vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)glsl";
    // RESIZE TEXTURE
    GLuint FB;
    GLuint RB;
    GLuint renderedTexture;
    GLIS_texture_buffer(FB, RB, renderedTexture, width_to, height_to);
    GLuint CHILDshaderProgram;
    GLuint CHILDvertexShader;
    GLuint CHILDfragmentShader;
    CHILDvertexShader = GLIS_createShader(GL_VERTEX_SHADER, CHILDvertexSource);
    CHILDfragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, CHILDfragmentSource);
    LOG_INFO("Creating Shader program");
    CHILDshaderProgram = GLIS_error_to_string_exec_GL(glCreateProgram());
    LOG_INFO("Attaching vertex Shader to program");
    GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDvertexShader));
    LOG_INFO("Attaching fragment Shader to program");
    GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDfragmentShader));
    LOG_INFO("Linking Shader program");
    GLIS_error_to_string_exec_GL(glLinkProgram(CHILDshaderProgram));
    LOG_INFO("Validating Shader program");
    GLboolean ProgramIsValid = GLIS_validate_program(CHILDshaderProgram);
    assert(ProgramIsValid == GL_TRUE);
    LOG_INFO("Using Shader program");
    GLIS_error_to_string_exec_GL(glUseProgram(CHILDshaderProgram));
    LOG_INFO("drawing rectangle");
    GLIS_draw_rectangle<GLint>(
        GL_TEXTURE0, renderedTexture, 0, 0, 0, width_to, height_to, width_from, height_from);
    LOG_INFO("drawn rectangle");
    TEXDATA_LEN = width_to * height_to * sizeof(GLuint);
    *TEXDATA = new GLuint[TEXDATA_LEN];
    memset(*TEXDATA, 0, TEXDATA_LEN);
    GLIS_error_to_string_exec_GL(glReadPixels(0, 0, width_to, height_to, GL_RGBA, GL_UNSIGNED_BYTE,
                                              *TEXDATA));
    GLIS_error_to_string_exec_GL(glDeleteProgram(CHILDshaderProgram));
    GLIS_error_to_string_exec_GL(glDeleteShader(CHILDfragmentShader));
    GLIS_error_to_string_exec_GL(glDeleteShader(CHILDvertexShader));
    GLIS_error_to_string_exec_GL(glDeleteTextures(1, &renderedTexture));
    GLIS_error_to_string_exec_GL(glDeleteRenderbuffers(1, &RB));
    GLIS_error_to_string_exec_GL(glDeleteFramebuffers(1, &FB));
    // restore
    GLIS_error_to_string_exec_GL(glBindFramebuffer(GL_READ_FRAMEBUFFER,
                                                   static_cast<GLuint>(backup.framebuffer.__GL_READ_FRAMEBUFFER_BINDING)));
    GLIS_error_to_string_exec_GL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                                                   static_cast<GLuint>(backup.framebuffer.__GL_DRAW_FRAMEBUFFER_BINDING)));
    GLIS_error_to_string_exec_GL(glBindRenderbuffer(GL_RENDERBUFFER,
                                                    static_cast<GLuint>(backup.renderbuffer.__GL_RENDERBUFFER_BINDING)));
    GLIS_error_to_string_exec_GL(
        glActiveTexture(static_cast<GLenum>(backup.texture.__GL_ACTIVE_TEXTURE)));
    GLIS_error_to_string_exec_GL(
        glBindVertexArray(static_cast<GLuint>(backup.texture.__GL_VERTEX_ARRAY_BINDING)));
    GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER,
                                              static_cast<GLuint>(backup.texture.__GL_ARRAY_BUFFER_BINDING)));
    GLIS_error_to_string_exec_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                                              static_cast<GLuint>(backup.texture.__GL_ELEMENT_ARRAY_BUFFER_BINDING)));
    GLIS_error_to_string_exec_GL(
        glUseProgram(static_cast<GLuint>(backup.program.__GL_CURRENT_PROGRAM)));
}

// fonts

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library GLIS_font;

bool GLIS_font_init() {
    if (FT_Init_FreeType(&GLIS_font)) {
        LOG_ERROR("ERROR::FREETYPE: Could not init FreeType Library");
        return false;
    }
    return true;
}

FT_Face GLIS_font_face;

bool GLIS_font_load(const char * font) {
    // load font from asset manager

    if (FT_New_Face(GLIS_font, font, 0, &GLIS_font_face)) {
        LOG_ERROR("ERROR::FREETYPE: Failed to load font");
        return false;
    }
    return true;
}

void GLIS_font_set_size(int width, int height) {
    FT_Set_Pixel_Sizes(GLIS_font_face, width, height);
}

struct Character {
    GLuint textureID;   // ID handle of the glyph texture
    glm::ivec2 size;    // Size of glyph
    glm::ivec2 bearing;  // Offset from baseline to left/top of glyph
    GLuint advance;    // Horizontal offset to advance to next glyph
};

#include <map>

std::map<GLchar, Character> sCharacters;

bool GLIS_font_store_ascii() {

    // Load first 128 characters of ASCII set
    int loaded = 0;
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(GLIS_font_face, c, FT_LOAD_RENDER)) {
            LOG_ERROR("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        loaded++;
        // Generate texture
        GLuint texture;
        GLIS_error_to_string_exec_GL(glGenTextures(1, &texture));
        GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, texture));
        GLIS_error_to_string_exec_GL(glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_R8,
                GLIS_font_face->glyph->bitmap.width,
                GLIS_font_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                GLIS_font_face->glyph->bitmap.buffer
        ));
        // Set texture options
        GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLIS_error_to_string_exec_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(GLIS_font_face->glyph->bitmap.width, GLIS_font_face->glyph->bitmap.rows),
                glm::ivec2(GLIS_font_face->glyph->bitmap_left, GLIS_font_face->glyph->bitmap_top),
                (GLuint) GLIS_font_face->glyph->advance.x
        };
        sCharacters.insert(std::pair<GLchar, Character>(c, character));
    }
    GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, 0));
    return loaded != 0;
}

void GLIS_font_free() {
    FT_Done_Face(GLIS_font_face);
    FT_Done_FreeType(GLIS_font);
}

bool GLIS_FONT_INITIALIZED = false;

bool GLIS_load_font(const char * font, int width, int height) {
    // TODO: smart load
    if (!GLIS_font_init()) return false;
    if (!GLIS_font_load(font)) {
        FT_Done_FreeType(GLIS_font);
        return false;
    }
    GLIS_font_set_size(width, height);

    // disable byte-alignment restriction
    GLIS_error_to_string_exec_GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    bool r = GLIS_font_store_ascii();

    // enable byte-alignment restriction
    GLIS_error_to_string_exec_GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

    if (!r) {
        GLIS_font_free();
        return false;
    }
    return true;
}

unsigned int GLIS_VAO, GLIS_VBO;

void GLIS_font_RenderText(GLfloat w, GLfloat h, std::string text, float x, float y, float scale, glm::vec3 color) {

    const char *CHILDvertexSource = R"glsl( #version 300 es

layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)glsl";

    const char *CHILDfragmentSource = R"glsl( #version 300 es
precision mediump float;

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)glsl";

    GLuint CHILDshaderProgram;
    GLuint CHILDvertexShader;
    GLuint CHILDfragmentShader;
    CHILDvertexShader = GLIS_createShader(GL_VERTEX_SHADER, CHILDvertexSource);
    CHILDfragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, CHILDfragmentSource);
    LOG_INFO("Creating Shader program");
    CHILDshaderProgram = GLIS_error_to_string_exec_GL(glCreateProgram());
    LOG_INFO("Attaching vertex Shader to program");
    GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDvertexShader));
    LOG_INFO("Attaching fragment Shader to program");
    GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDfragmentShader));
    LOG_INFO("Linking Shader program");
    GLIS_error_to_string_exec_GL(glLinkProgram(CHILDshaderProgram));
    LOG_INFO("Validating Shader program");
    GLboolean ProgramIsValid = GLIS_validate_program(CHILDshaderProgram);
    assert(ProgramIsValid == GL_TRUE);

    if (CHILDshaderProgram) {
        // Configure VAO/VBO for texture quads
        glGenVertexArrays(1, &GLIS_VAO);
        glGenBuffers(1, &GLIS_VBO);
        glBindVertexArray(GLIS_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, GLIS_VAO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    LOG_INFO("Using Shader program");
    GLIS_error_to_string_exec_GL(glUseProgram(CHILDshaderProgram));

    glm::mat4 projection = glm::ortho(0.0f, w, 0.0f, h);

    GLuint loc = GLIS_error_to_string_exec_GL(glGetUniformLocation(CHILDshaderProgram, "projection"));
    GLIS_error_to_string_exec_GL(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection)));

    loc = GLIS_error_to_string_exec_GL(glGetUniformLocation(CHILDshaderProgram, "textColor"));
    GLIS_error_to_string_exec_GL(glUniform3f(loc, color.x, color.y, color.z));

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLIS_error_to_string_exec_GL(glActiveTexture(GL_TEXTURE0));
    GLIS_error_to_string_exec_GL(glBindVertexArray(GLIS_VAO));

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {

        if (sCharacters.count(*c) > 0) {

            Character ch = sCharacters[*c];

            GLfloat xpos = x + ch.bearing.x * scale;
            GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

            GLfloat w = ch.size.x * scale;
            GLfloat h = ch.size.y * scale;
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                    {xpos,     ypos + h, 0.0f, 0.0f},
                    {xpos,     ypos,     0.0f, 1.0f},
                    {xpos + w, ypos,     1.0f, 1.0f},

                    {xpos,     ypos + h, 0.0f, 0.0f},
                    {xpos + w, ypos,     1.0f, 1.0f},
                    {xpos + w, ypos + h, 1.0f, 0.0f}
            };

            //#################debug#################
//            static bool debug = false;
//            if (!debug) {
//                glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(sWidth), 0.0f,
//                                                  static_cast<GLfloat>(sHeight));
//                glm::vec4 v(xpos, ypos + h, 0.0f, 0.0f);
//                v = projection * v;
//                base_LOG("%f,%f,%f,%f", v.x, v.y, v.z, v.w);
//                debug = true;
//            }
            //#############end debug#################

            // Render glyph texture over quad
            GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, ch.textureID));
            // Update content of VBO memory
            GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER, GLIS_VBO));
            GLIS_error_to_string_exec_GL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices),
                            vertices)); // Be sure to use glBufferSubData and not glBufferData

            GLIS_error_to_string_exec_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
            // Render quad
            GLIS_error_to_string_exec_GL(glDrawArrays(GL_TRIANGLES, 0, 6));
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.advance >> 6) *
                 scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    GLIS_error_to_string_exec_GL(glBindVertexArray(0));
    GLIS_error_to_string_exec_GL(glBindTexture(GL_TEXTURE_2D, 0));

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    GLIS_error_to_string_exec_GL(glDeleteProgram(CHILDshaderProgram));
    GLIS_error_to_string_exec_GL(glDeleteShader(CHILDfragmentShader));
    GLIS_error_to_string_exec_GL(glDeleteShader(CHILDvertexShader));
}

#include "GLIS_COMMANDS.h"

#endif //GLNE_GLIS_H
