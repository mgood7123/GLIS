//
// Created by konek on 8/14/2019.
//

#include <glis/glis.hpp>

GLIS_CLASS G;
GLIS glis;

int main() {
    int W = 1000;
    int H = 1000;
    if (glis.GLIS_setupOffScreenRendering(G, W, H)) {
        // create a new texture
        GLuint FB;
        GLuint RB;
        GLuint texture;
        glis.GLIS_texture_buffer(FB, RB, texture, W, H);

        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;
        glis.GLIS_build_simple_shader_program(vertexShader, fragmentShader, shaderProgram);
        glUseProgram(shaderProgram);
        glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, W, H, W, H);
        double program_start = now_ms();
        LOG_INFO("creating 21 windows");
        LOG_INFO("creating window %d", 0);
        size_t win_id1 = glis.GLIS_new_window(0, 0, W, H);
        LOG_INFO("window id: %zu", win_id1);
        SERVER_LOG_TRANSFER_INFO = true;
        glis.GLIS_upload_texture(G, win_id1, texture, W, H);
        LOG_INFO("created window %d", 0);
        for (int i = 0; i < 20; i++) {
            LOG_INFO("creating window %d", i + 1);
            int s = 50;
            size_t win_id2 = glis.GLIS_new_window(i * s, i * s, s, s);
            LOG_INFO("window id: %zu", win_id2);
            glis.GLIS_upload_texture(G, win_id2, texture, W, H);
            LOG_INFO("created window %d", i + 1);
        }
        double end = now_ms();
        LOG_INFO("created 21 windows in %G milliseconds", end - program_start);

        LOG_INFO("Cleaning up");
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &RB);
        glDeleteFramebuffers(1, &FB);
        glis.GLIS_destroy_GLIS(G);
        LOG_INFO("Destroyed sub Compositor GLIS");
        LOG_INFO("Cleaned up");
    }
    return 0;
}