//
// Created by smallville7123 on 12/07/20.
//

#include <glis/backup/backup.hpp>

void GLIS_BACKUP::backup() {
    framebuffer.backup();
    renderbuffer.backup();
    texture.backup();
    program.backup();
}

void GLIS_BACKUP::restore() {
    framebuffer.restore();
    renderbuffer.restore();
    texture.restore();
    program.restore();
}