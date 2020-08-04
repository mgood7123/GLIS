//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <Magnum/GL/OpenGL.h>

class GLIS_BACKUP_RENDERBUFFER {
    GLint __GL_RENDERBUFFER_BINDING = -1, __GL_RENDERBUFFER_WIDTH = -1,
            __GL_RENDERBUFFER_HEIGHT = -1, __GL_RENDERBUFFER_INTERNAL_FORMAT = -1,
            __GL_RENDERBUFFER_RED_SIZE = -1, __GL_RENDERBUFFER_GREEN_SIZE = -1,
            __GL_RENDERBUFFER_BLUE_SIZE = -1, __GL_RENDERBUFFER_ALPHA_SIZE = -1,
            __GL_RENDERBUFFER_DEPTH_SIZE = -1, __GL_RENDERBUFFER_STENCIL_SIZE = -1,
            __GL_RENDERBUFFER_SAMPLES = -1;
public:
    void backup();

    void restore();
};
