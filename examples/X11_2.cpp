//
// Created by smallville7123 on 14/07/20.
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

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;
GLuint texture;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, 400,400, 400, 400);
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glViewport(0, 0, renderer.width, renderer.height);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteTextures(1, &texture);
    glis.destroyX11Window(screen);
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 400, 400);
    glis.GLIS_setupOnScreenRendering(screen);
    glis.GLIS_texture(texture);
    glis.GLIS_build_simple_shader_program(
            vertexShader, vertexSource, fragmentShader, fragmentSource, shaderProgram
    );
    glUseProgram(shaderProgram);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
