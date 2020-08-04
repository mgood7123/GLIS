//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <Magnum/GL/OpenGL.h>

class GLIS_BACKUP_TEXTURE {
    GLint __GL_ACTIVE_TEXTURE = -1, __GL_TEXTURE_BUFFER_BINDING = -1,
            __GL_VERTEX_ARRAY_BINDING = -1, __GL_ARRAY_BUFFER_BINDING = -1,
            __GL_ELEMENT_ARRAY_BUFFER_BINDING = -1;
public:
    void backup();

    void restore();
};
