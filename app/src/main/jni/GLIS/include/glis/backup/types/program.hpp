//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <GLES3/gl32.h> // in ubuntu, this header is provided by libgles-dev

class GLIS_BACKUP_PROGRAM {
    GLint __GL_CURRENT_PROGRAM;
public:
    void backup();

    void restore();
};
