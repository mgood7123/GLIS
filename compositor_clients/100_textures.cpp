//
// Created by konek on 8/14/2019.
//

#include <glis/glis.hpp>

GLIS_CLASS G;
GLIS glis;

int main() {
    glis.GLIS_INIT_SHARED_MEMORY(1080,720);
    if (glis.GLIS_setupOffScreenRendering(G, 1080,720)) {
        // create a new texture
        GLuint FB;
        GLuint RB;
        GLuint texture;
        glis.GLIS_texture_buffer(FB, RB, texture, 1080,720);

        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;
        glis.GLIS_build_simple_shader_program(vertexShader, fragmentShader, shaderProgram);
        glUseProgram(shaderProgram);

        glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, 1080,720);

        for (int i = 0; i < 100; i++) {
            glis.GLIS_upload_texture(G, glis.GLIS_new_window(0, 0, 1080,720), texture, 1080,720);
        }

        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &RB);
        glDeleteFramebuffers(1, &FB);
        glis.GLIS_destroy_GLIS(G);
    }
    return 0;
}