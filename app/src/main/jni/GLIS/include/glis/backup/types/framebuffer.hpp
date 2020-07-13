//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <GLES3/gl32.h> // in ubuntu, this header is provided by libgles-dev

class GLIS_BACKUP_FRAMEBUFFER {
    GLint __GL_READ_FRAMEBUFFER_BINDING, __GL_DRAW_FRAMEBUFFER_BINDING;
public:
    void backup();

    void restore();
};