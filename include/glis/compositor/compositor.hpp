// LINUX

#pragma once

#include <glis/glis.hpp>

extern GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(GLIS_COMPOSITOR_DEFAULT_DRAW_FUNCTION, glis, CompositorMain, font, fps);

extern GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(GLIS_COMPOSITOR_DEFAULT_RESIZE_FUNCTION, glis, CompositorMain, font, fps);

extern GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(GLIS_COMPOSITOR_DEFAULT_CLOSE_FUNCTION, glis, CompositorMain, font, fps);

extern void GLIS_COMPOSITOR_BEFORE_REQUEST_STARTUP(GLIS & glis);

extern void GLIS_COMPOSITOR_REQUEST_STARTUP(GLIS & glis);

extern void GLIS_COMPOSITOR_HANDLE_STARTUP_REQUEST(GLIS & glis);

extern void GLIS_COMPOSITOR_DO_MAIN(
        GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FPS & fps,
        GLIS_FONT & font, const char * font_path, int font_size,
        void (*compositorLoop) (
                GLIS & glis,
                GLIS_CLASS & glis_class,
                GLIS_FONT & font,
                GLIS_FPS & fps,
                GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
                GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
                GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
        ),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowDraw),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowResize),
        GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(onWindowClose)
);

extern void GLIS_COMPOSITOR_REQUEST_SHUTDOWN(GLIS & glis, GLIS_CLASS & CompositorMain);

extern void GLIS_COMPOSITOR_REQUEST_SHUTDOWN_MT(GLIS & glis, GLIS_CLASS & CompositorMain);

extern void GLIS_COMPOSITOR_HANDLE_SHUTDOWN_REQUEST(
        GLIS &glis, GLIS_CLASS &CompositorMain,
        GLuint & vertexShader, GLuint & fragmentShader, GLuint & shaderProgram
);

#define GLIS_COMPOSITOR_LOOP_FUNCTION(functionName, GLIS_name, GLIS_CLASS_name, GLIS_FONT_name, GLIS_FPS_name, draw_function_name, resize_function_name, close_function_name) void functionName (GLIS & GLIS_name, GLIS_CLASS & GLIS_CLASS_name, GLIS_FONT & GLIS_FONT_name, GLIS_FPS & GLIS_FPS_name, GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(draw_function_name), GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(resize_function_name), GLIS_CALLBACKS_DRAW_RESIZE_CLOSE_PARAMATER(close_function_name))