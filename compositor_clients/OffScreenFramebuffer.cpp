//
// Created by konek on 8/18/2019.
//

#include <glis/glis.hpp>

GLIS_CLASS G;

GLIS glis;



int main() {
    int W = GLIS_COMMON_WIDTH;
    int H = GLIS_COMMON_HEIGHT;
    if (glis.GLIS_setupOffScreenRendering(G, W, H)) {
        GLuint vertexShader;
        GLuint fragmentShader;
        GLuint shaderProgram;
        GLuint framebuffer;
        GLuint renderbuffer;
        GLuint texture;
        glis.GLIS_texture_buffer(framebuffer, renderbuffer, texture, G.width, G.height);
        glis.GLIS_build_simple_shader_program(vertexShader, fragmentShader, shaderProgram);
        glUseProgram(shaderProgram);
        glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, W,H, W, H);
        LOG_INFO("creating window %d", 0);
        size_t win_id1 = glis.GLIS_new_window(0, 0, W, H);
        LOG_INFO("window id: %zu", win_id1);
        SERVER_LOG_TRANSFER_INFO = true;
        glis.GLIS_upload_texture(G, win_id1, texture, W, H);
        LOG_INFO("created window %d", 0);

        LOG_INFO("Cleaning up");
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &renderbuffer);
        glDeleteFramebuffers(1, &framebuffer);
        glis.GLIS_destroy_GLIS(G);
        LOG_INFO("Destroyed sub Compositor GLIS");
        LOG_INFO("Cleaned up");
    }
    return 0;
}