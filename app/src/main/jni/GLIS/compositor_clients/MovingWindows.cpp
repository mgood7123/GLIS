//
// Created by konek on 8/14/2019.
//

#include <glis/glis.hpp>

const char *vertexSource = R"glsl( #version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)glsl";

const char *fragmentSource = R"glsl( #version 300 es
out highp vec4 FragColor;
in highp vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

GLIS_CLASS G;

GLIS glis;

int main() {
    int W = GLIS_COMMON_WIDTH;
    int H = GLIS_COMMON_HEIGHT;
    if (glis.GLIS_setupOffScreenRendering(G, W, H)) {
        // create a new texture
        GLuint FB;
        GLuint RB;
        GLuint texture;
        glis.GLIS_texture_buffer(FB, RB, texture, W, H);

        GLuint shaderProgram;
        GLuint vertexShader;
        GLuint fragmentShader;

        glis.GLIS_build_simple_shader_program(
                vertexShader, vertexSource, fragmentShader, fragmentSource, shaderProgram
        );
        glUseProgram(shaderProgram);
        LOG_INFO("A: drawing rectangle");
        glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, W, H, W, H);
        LOG_INFO("A: drawn rectangle");

        size_t win_id1 = glis.GLIS_new_window(500, 500, 200, 200);
        glis.GLIS_upload_texture(G, win_id1, texture, W, H);
        LOG_INFO("A: win_id1 = %zu", win_id1);
        size_t win_id2 = glis.GLIS_new_window(600, 600, 200, 200);
        glis.GLIS_upload_texture(G, win_id2, texture, W, H);
        LOG_INFO("A: win_id2 = %zu", win_id2);
        for (int i = 500; i <= 600; i++) glis.GLIS_modify_window(win_id1, 500, i, 200, 200);
        for (int i = 600; i <= 700; i++) glis.GLIS_modify_window(win_id2, i, 600, 200, 200);
        for (int i = 599; i >= 451; i--) glis.GLIS_modify_window(win_id1, 500, i, 200, 200);
        for (int i = 699; i >= 501; i--) glis.GLIS_modify_window(win_id2, i, 600, 200, 200);
        LOG_INFO("A: Cleaning up");
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &RB);
        glDeleteFramebuffers(1, &FB);
        glis.GLIS_destroy_GLIS(G);
        LOG_INFO("A: Destroyed sub Compositor GLIS");
        LOG_INFO("A: Cleaned up");
    }
    return 0;
}