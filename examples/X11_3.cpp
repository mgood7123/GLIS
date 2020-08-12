//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>



GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;
GLuint vertexShader2;
GLuint fragmentShader2;
GLuint shaderProgram2;
GLuint framebuffer;
GLuint texture;
GLuint rbo;

const char *GLIS_COMPOSITOR_VERTEX_SOURCE = R"glsl( #version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = vec4(aColor, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char *GLIS_COMPOSITOR_FRAGMENT_SOURCE = R"glsl( #version 300 es
out highp vec4 FragColor;

in highp vec4 ourColor;
in highp vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)glsl";

// void texture_data[400*400];

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    // draw a texture

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); glis.GLIS_error_to_string_GL("glBindFramebuffer");
    glUseProgram(shaderProgram);
    glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, 400,400, 400, 400);

    // draw the texture to the screen

    glBindFramebuffer(GL_FRAMEBUFFER, 0); glis.GLIS_error_to_string_GL("glBindFramebuffer");
    glUseProgram(shaderProgram2);
    glis.GLIS_draw_rectangle<GLint>(GL_TEXTURE0, texture, 0, 0, 0, 400,400, 400, 400);
    
    // swap buffers
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glis.GLIS_Viewport(renderer);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);
    glis.destroyX11Window(screen);
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    GLIS_ABORT_ON_ERROR = true;
    GLIS_ABORT_ON_DEBUG_LEVEL_API = false;
    glis.getX11Window(screen, 400, 400);
    glis.GLIS_setupOnScreenRendering(screen);
    glis.GLIS_texture_buffer(framebuffer, rbo, texture, screen.width, screen.height);
    glis.GLIS_build_simple_shader_program(vertexShader, fragmentShader, shaderProgram);
    glis.GLIS_build_simple_shader_program(
        vertexShader2, GLIS_COMPOSITOR_VERTEX_SOURCE,
        fragmentShader2, GLIS_COMPOSITOR_FRAGMENT_SOURCE,
        shaderProgram2
    );
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
