//
// Created by konek on 8/18/2019.
//

#include "../GLIS.h"

class GLIS_CLASS G;
int main() {
    int W = 1080;
    int H = 2031;
    if (GLIS_setupOffScreenRendering(G, W, H)) {
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
//        GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, 0, 0, W, H, W, H);
        LOG_INFO("creating window %d", 0);
        size_t win_id1 = GLIS_new_window(0, 0, W, H);
        LOG_INFO("window id: %zu", win_id1);
        SERVER_LOG_TRANSFER_INFO = true;
        GLIS_upload_texture(G, win_id1, GLIS_current_texture, W, H);
        LOG_INFO("created window %d", 0);

        LOG_INFO("Cleaning up");
        GLIS_destroy_GLIS(G);
        LOG_INFO("Destroyed sub Compositor GLIS");
        LOG_INFO("Cleaned up");
    }
    return 0;
}