//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <Magnum/GL/OpenGL.h>

class GLIS_BACKUP_RENDERBUFFER {
    GLint __GL_RENDERBUFFER_BINDING, __GL_RENDERBUFFER_WIDTH, __GL_RENDERBUFFER_HEIGHT,
            __GL_RENDERBUFFER_INTERNAL_FORMAT, __GL_RENDERBUFFER_RED_SIZE,
            __GL_RENDERBUFFER_GREEN_SIZE, __GL_RENDERBUFFER_BLUE_SIZE,
            __GL_RENDERBUFFER_ALPHA_SIZE, __GL_RENDERBUFFER_DEPTH_SIZE,
            __GL_RENDERBUFFER_STENCIL_SIZE, __GL_RENDERBUFFER_SAMPLES;
public:
    void backup();

    void restore();
};