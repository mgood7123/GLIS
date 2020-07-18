//
// Created by smallville7123 on 12/07/20.
//

#include <glis/backup/types/texture.hpp>

void GLIS_BACKUP_TEXTURE::backup() {
    glGetIntegerv(GL_ACTIVE_TEXTURE, &__GL_ACTIVE_TEXTURE);
//    glGetIntegerv(GL_TEXTURE_BUFFER_BINDING, &__GL_TEXTURE_BUFFER_BINDING);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &__GL_VERTEX_ARRAY_BINDING);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &__GL_ARRAY_BUFFER_BINDING);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &__GL_ELEMENT_ARRAY_BUFFER_BINDING);
}

void GLIS_BACKUP_TEXTURE::restore() {
    glActiveTexture(__GL_ACTIVE_TEXTURE);
    // assume 2D
    glBindTexture(GL_TEXTURE_2D, __GL_TEXTURE_BUFFER_BINDING);
    glBindVertexArray(__GL_VERTEX_ARRAY_BINDING);
    glBindBuffer(GL_ARRAY_BUFFER, __GL_ARRAY_BUFFER_BINDING);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __GL_ELEMENT_ARRAY_BUFFER_BINDING);
}