//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

void noop() {}

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

int main() {
    GLIS_CLASS CompositorMain;
    GLIS glis;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);

    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rbo;
    glis.GLIS_texture_buffer_linux(framebuffer, rbo, textureColorbuffer, CompositorMain.width, CompositorMain.height);
    glis.GLIS_build_simple_shader_program(
            vertexShader, vertexSource, fragmentShader, fragmentSource, shaderProgram
    );
    glUseProgram(shaderProgram);
    glis.GLIS_draw_high_resolution_square();
    glis.GLIS_SwapBuffers(CompositorMain);
    glis.runUntilX11WindowClose(CompositorMain, noop);
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);
    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
}
