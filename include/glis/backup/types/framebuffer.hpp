//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <Magnum/GL/OpenGL.h>

class GLIS_BACKUP_FRAMEBUFFER {
    GLint __GL_READ_FRAMEBUFFER_BINDING = -1, __GL_DRAW_FRAMEBUFFER_BINDING = -1;
public:
    void backup();

    void restore();
};