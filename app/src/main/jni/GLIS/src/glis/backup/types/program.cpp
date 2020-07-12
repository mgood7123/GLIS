//
// Created by smallville7123 on 12/07/20.
//

#include <glis/backup/types/program.hpp>

void GLIS_BACKUP_PROGRAM::backup() {
    glGetIntegerv(GL_CURRENT_PROGRAM, &__GL_CURRENT_PROGRAM);
}

void GLIS_BACKUP_PROGRAM::restore() {
    glUseProgram(__GL_CURRENT_PROGRAM);
}

