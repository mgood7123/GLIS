//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>
#include "../include/glis/internal/context.hpp"

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
    glis.getX11Window(CompositorMain, 400, 400);
    glis.GLIS_setupOnScreenRendering(CompositorMain);

    GLuint FB;
    GLuint RB;
    GLuint renderedTexture;
    
//    glis.GLIS_texture_buffer(FB, RB, renderTexture, CompositorMain.width, CompositorMain.height);

    // gen texture buffer
    GLuint &framebuffer = FB;
    GLuint &renderbuffer = RB;
    GLint &texture_width = CompositorMain.width;
    GLint &texture_height = CompositorMain.height;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8UI, texture_width, texture_height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                              renderbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    GLenum FramebufferStatus =
            glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (FramebufferStatus != GL_FRAMEBUFFER_COMPLETE)
        LOG_ERROR("framebuffer is not complete");
    else
        LOG_INFO("framebuffer is complete");

    // create a new texture
    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, 0);

    glGenerateMipmap(GL_TEXTURE_2D); // this DOES NOT affect the total size of read pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // set up shader program
    GLuint shaderProgram;
    GLuint vertexShader;
    GLuint fragmentShader;
    vertexShader = glis.GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
    fragmentShader = glis.GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    GLboolean ProgramIsValid = glis.GLIS_validate_program(shaderProgram);
    assert(ProgramIsValid == GL_TRUE);
    glUseProgram(shaderProgram);

    // set texture
    glis.GLIS_set_texture(GL_TEXTURE0, renderedTexture);

    // draw to texture
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // somehow draw framebuffer texture to screen

    // swap buffers
    glis.GLIS_SwapBuffers(CompositorMain);


    glis.runUntilX11WindowClose(CompositorMain, noop);

    glis.destroyX11Window(CompositorMain);
    glis.GLIS_destroy_GLIS(CompositorMain);
}