//
// Created by smallville7123 on 12/07/20.
//

#include <glis/backup/types/framebuffer.hpp>

void GLIS_BACKUP_FRAMEBUFFER::backup() {
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &__GL_READ_FRAMEBUFFER_BINDING);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &__GL_DRAW_FRAMEBUFFER_BINDING);
//    glGetFramebufferParameteriv()
//    glGetFramebufferAttachmentParameteriv()
}

void GLIS_BACKUP_FRAMEBUFFER::restore() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, __GL_READ_FRAMEBUFFER_BINDING);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, __GL_DRAW_FRAMEBUFFER_BINDING);
}
