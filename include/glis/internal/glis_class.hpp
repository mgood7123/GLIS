#pragma once

#include <Magnum/GL/OpenGL.h>
#include <Magnum/Platform/WindowlessEglApplication.h> // EGL
#include <Magnum/Platform/GLContext.h>
#include <glis/internal/log.hpp>
#include <glis/ipc/server_core.hpp>
#include "fps.hpp"

constexpr static int GLIS_COMMON_WIDTH = 1080;
constexpr static int GLIS_COMMON_HEIGHT = 2032;

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
            init_eglMakeCurrent = false,
            init_debug = false;
    const GLint
            *configuration_attributes = nullptr,
            *context_attributes = nullptr,
            *surface_attributes = nullptr;
    EGLint
            eglMajVers = 0,
            eglMinVers = 0,
            number_of_configurations = 0;
    EGLNativeDisplayType display_id = EGL_DEFAULT_DISPLAY;
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLConfig configuration = 0;
    EGLContext
            context = EGL_NO_CONTEXT,
            shared_context = EGL_NO_CONTEXT;
    Magnum::Platform::GLContext contextMagnum {Magnum::NoCreate};
    int EGL_CONTEXT_CLIENT_VERSION_ = 3;
    bool debug_context = false;
    EGLSurface surface = EGL_NO_SURFACE;
    // previously: ANativeWindow *native_window = nullptr;
    EGLNativeWindowType native_window = 0;
    GLint
            width = 0,
            height = 0;
    int dpi = 0;
    SOCKET_SERVER server;
    Kernel KERNEL;
    
    bool GLIS_LOG_PRINT_CONVERSIONS = true;
    
    template<typename TYPE> float GLIS_inverse(TYPE num) {
        return num < 0 ? -num : -(num);
    }

    template<typename TYPE> float GLIS_convert(TYPE num, TYPE num_max) {
        // 3 year old magic
        return (num - (num_max / 2)) / (num_max / 2);
    }
    
    template<typename TYPE> class GLIS_coordinates {
    public:
        GLIS_coordinates() {}
        GLIS_coordinates(TYPE TYPE_INITIALIZER) {
            TYPE x = TYPE_INITIALIZER;
            TYPE y = TYPE_INITIALIZER;
        }

        TYPE x;
        TYPE y;
    };
    
    struct normalized_device_coordinate {
        float x = 0.0f;
        float y = 0.0f;
    };
    
    constexpr static int GLIS_CONVERSION_ORIGIN_TOP_LEFT = 0;
    constexpr static int GLIS_CONVERSION_ORIGIN_TOP_RIGHT = 1;
    constexpr static int GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT = 2;
    constexpr static int GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT = 3;

    int GLIS_CONVERSION_ORIGIN = GLIS_CONVERSION_ORIGIN_TOP_LEFT;

    template<typename TYPEFROM, typename TYPETO>
    class GLIS_coordinates<TYPETO> pixel_location_to_normalized_device_coordinate(TYPETO TYPETO_INITIALIZER, TYPEFROM x, TYPEFROM y, TYPEFROM x_max, TYPEFROM y_max, bool clip) {
        class GLIS_coordinates<TYPETO> xy(TYPETO_INITIALIZER);
        if (x > x_max) {
            if (GLIS_LOG_PRINT_CONVERSIONS)
                LOG_INFO("x is out of bounds (expected %hi, got %hi)", x_max, x);
            if (clip) {
                if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", x_max);
                x = x_max;
            }
        } else if (x < 0) {
            if (GLIS_LOG_PRINT_CONVERSIONS)
                LOG_INFO("x is out of bounds (expected %hi, got %hi)", 0, x);
            if (clip) {
                if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", 0);
                x = 0;
            }
        }
        if (y > y_max) {
            if (GLIS_LOG_PRINT_CONVERSIONS)
                LOG_INFO("y is out of bounds (expected %hi, got %hi)", y_max, y);
            if (clip) {
                if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", y_max);
                y = y_max;
            }
        } else if (y < 0) {
            if (GLIS_LOG_PRINT_CONVERSIONS)
                LOG_INFO("y is out of bounds (expected %hi, got %hi)", 0, y);
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
            LOG_INFO(
                    "width: %hi, width_max: %hi, height: %hi, height_max: %hi, ConvertPair: %f, %f",
                    x, x_max, y, y_max, xy.x, xy.y);
        return xy;
    }

    const struct normalized_device_coordinate
    pixel_location_to_normalized_device_coordinate(int x, int y);
};
