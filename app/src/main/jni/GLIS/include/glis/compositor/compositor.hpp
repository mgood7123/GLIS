// LINUX

#pragma once

#include <glis/glis.hpp>

extern void GLIS_COMPOSITOR_DEFAULT_DRAW_FUNCTION(GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FONT & font, GLIS_FPS & fps);

extern void GLIS_COMPOSITOR_DEFAULT_RESIZE_FUNCTION(GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FONT & font, GLIS_FPS & fps, GLsizei width, GLsizei height);

extern void GLIS_COMPOSITOR_DEFAULT_CLOSE_FUNCTION(GLIS & glis, GLIS_CLASS & CompositorMain, GLIS_FONT & font, GLIS_FPS & fps);

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
                void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &),
                void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei),
                void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &)
        ),
        void (*draw)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &),
        void (*onWindowResize)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei),
        void (*onWindowClose)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &)
);

extern void GLIS_COMPOSITOR_REQUEST_SHUTDOWN(GLIS & glis, GLIS_CLASS & CompositorMain);

extern void GLIS_COMPOSITOR_REQUEST_SHUTDOWN_MT(GLIS & glis, GLIS_CLASS & CompositorMain);

extern void GLIS_COMPOSITOR_HANDLE_SHUTDOWN_REQUEST(
        GLIS &glis, GLIS_CLASS &CompositorMain,
        GLuint & vertexShader, GLuint & fragmentShader, GLuint & shaderProgram
);

#define GLIS_COMPOSITOR_LOOP_FUNCTION(functionName, GLIS_name, GLIS_CLASS_name, GLIS_FONT_name, GLIS_FPS_name, draw_function_name, resize_function_name, close_function_name) void functionName (GLIS & GLIS_name, GLIS_CLASS & GLIS_CLASS_name, GLIS_FONT & GLIS_FONT_name, GLIS_FPS & GLIS_FPS_name, void (*draw_function_name)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &), void (*resize_function_name)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &, GLsizei, GLsizei), void (*close_function_name)(class GLIS &, class GLIS_CLASS &, class GLIS_FONT &, class GLIS_FPS &))