//
// Created by smallville7123 on 12/07/20.
//

#pragma once

#include <glis/backup/types/framebuffer.hpp>
#include <glis/backup/types/renderbuffer.hpp>
#include <glis/backup/types/texture.hpp>
#include <glis/backup/types/program.hpp>

#include <glis/backup/types/framebuffer.hpp>
#include <glis/backup/types/renderbuffer.hpp>
#include <glis/backup/types/texture.hpp>
#include <glis/backup/types/program.hpp>

class GLIS_BACKUP {
public:
    GLIS_BACKUP_FRAMEBUFFER framebuffer;
    GLIS_BACKUP_RENDERBUFFER renderbuffer;
    GLIS_BACKUP_TEXTURE texture;
    GLIS_BACKUP_PROGRAM program;

    void backup();

    void restore();
};
