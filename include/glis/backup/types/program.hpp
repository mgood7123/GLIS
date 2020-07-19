//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <Magnum/GL/OpenGL.h>

class GLIS_BACKUP_PROGRAM {
    GLint __GL_CURRENT_PROGRAM;
public:
    void backup();

    void restore();
};
