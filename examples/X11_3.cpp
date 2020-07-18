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

GLIS_CLASS CompositorMain;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;
GLuint framebuffer;
GLuint texture;
GLuint rbo;

GLIS_CALLBACKS_DRAW(draw, glis, renderer, font, fps) {
    glis.GLIS_error_to_string_GL("before glis.GLIS_draw_rectangle");
    glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, 400,400, 400, 400);
    glis.GLIS_error_to_string_GL("after glis.GLIS_draw_rectangle");
    glis.GLIS_SwapBuffers(CompositorMain);
    glis.GLIS_error_to_string_GL("after glis.GLIS_SwapBuffers");
}

GLIS_CALLBACKS_RESIZE(resize, glis, renderer, font, fps, width, height) {
    glViewport(0, 0, width, height);
}

GLIS_CALLBACKS_CLOSE(close, glis, renderer, font, fps) {
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);
    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
}

int main() {
    GLIS_ABORT_ON_ERROR = true;
    GLIS_ABORT_ON_DEBUG_LEVEL_API = false;
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);
    glis.GLIS_error_to_string_GL("after glis.GLIS_setupOnScreenRendering");
    glis.GLIS_texture_buffer(framebuffer, rbo, texture, CompositorMain.width, CompositorMain.height);
    glis.GLIS_error_to_string_GL("after glis.GLIS_texture_buffer");
    glis.GLIS_build_simple_shader_program(
            vertexShader, vertexSource, fragmentShader, fragmentSource, shaderProgram
    );
    glUseProgram(shaderProgram);
    glis.GLIS_error_to_string_GL("glUseProgram");
    glis.runUntilX11WindowClose(glis, CompositorMain, font, fps, draw, resize, close);
}
