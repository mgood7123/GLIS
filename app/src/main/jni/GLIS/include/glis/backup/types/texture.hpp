//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <GLES3/gl32.h>

class GLIS_BACKUP_TEXTURE {
    GLint __GL_ACTIVE_TEXTURE, __GL_TEXTURE_BUFFER_BINDING, __GL_VERTEX_ARRAY_BINDING,
            __GL_ARRAY_BUFFER_BINDING, __GL_ELEMENT_ARRAY_BUFFER_BINDING;
public:
    void backup();

    void restore();
};
