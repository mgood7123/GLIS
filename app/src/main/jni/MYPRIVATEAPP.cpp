//
// Created by konek on 8/14/2019.
//

#include "GLIS.h"

const char *CHILDvertexSource = R"glsl( #version 320 es
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

const char *CHILDfragmentSource = R"glsl( #version 320 es
out highp vec4 FragColor;
in highp vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

class GLIS_CLASS G;
int main() {
//    SERVER_LOG_TRANSFER_INFO = true;
//    GLIS_LOG_PRINT_SHAPE_INFO = true;
//    GLIS_LOG_PRINT_VERTEX = true;
//    GLIS_LOG_PRINT_CONVERSIONS = true;
//    GLIS_LOG_PRINT_NON_ERRORS = true;
//    GLIS_ERROR_PRINTING_TYPE = GLIS_ERROR_PRINTING_TYPE_CODE;
    int W = 1080;
    int H = 2031;
    if (GLIS_setupOffScreenRendering(G, W, H)) {
        GLIS_error_to_string();
        // create a new texture
        GLuint FB;
        GLuint RB;
        GLuint renderedTexture;
        GLIS_texture_buffer(FB, RB, renderedTexture, W, H);

        GLuint CHILDshaderProgram;
        GLuint CHILDvertexShader;
        GLuint CHILDfragmentShader;
        CHILDvertexShader = GLIS_createShader(GL_VERTEX_SHADER, CHILDvertexSource);
        CHILDfragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, CHILDfragmentSource);
        LOG_INFO("Creating Shader program");
        CHILDshaderProgram = GLIS_error_to_string_exec_GL(glCreateProgram());
        LOG_INFO("Attaching vertex Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDvertexShader));
        LOG_INFO("Attaching fragment Shader to program");
        GLIS_error_to_string_exec_GL(glAttachShader(CHILDshaderProgram, CHILDfragmentShader));
        LOG_INFO("Linking Shader program");
        GLIS_error_to_string_exec_GL(glLinkProgram(CHILDshaderProgram));
        LOG_INFO("Validating Shader program");
        GLboolean ProgramIsValid = GLIS_validate_program(CHILDshaderProgram);
        assert(ProgramIsValid == GL_TRUE);

        LOG_INFO("Using Shader program");
        GLIS_error_to_string_exec_GL(glUseProgram(CHILDshaderProgram));
//        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//        glClear(GL_COLOR);
        GLIS_draw_rectangle<GLint>(GL_TEXTURE0, renderedTexture, 0, 0, 0, W, H, W, H);

        LOG_INFO("creating 201 windows");
        LOG_INFO("creating window %d", 0);
        size_t win_id1 = GLIS_new_window(0, 0, W, H);
        LOG_INFO("window id: %zu", win_id1);
        SERVER_LOG_TRANSFER_INFO = true;
        GLIS_upload_texture(G, win_id1, renderedTexture, W, H);
        LOG_INFO("created window %d", 0);
        for (int i = 0; i < 20; i++) {
            LOG_INFO("creating window %d", i + 1);
            int s = 50;
            GLint ii = i * s;
            size_t win_id2 = GLIS_new_window(ii, ii, ii + s, ii + s);
            LOG_INFO("window id: %zu", win_id2);
            GLIS_upload_texture_resize(G, win_id2, renderedTexture, W, H, s, s);
            LOG_INFO("creating window %d", i + 1);
        }
        LOG_INFO("created 201 windows");

        LOG_INFO("Cleaning up");
        GLIS_error_to_string_exec_GL(glDeleteProgram(CHILDshaderProgram));
        GLIS_error_to_string_exec_GL(glDeleteShader(CHILDfragmentShader));
        GLIS_error_to_string_exec_GL(glDeleteShader(CHILDvertexShader));
        GLIS_error_to_string_exec_GL(glDeleteTextures(1, &renderedTexture));
        GLIS_error_to_string_exec_GL(glDeleteRenderbuffers(1, &RB));
        GLIS_error_to_string_exec_GL(glDeleteFramebuffers(1, &FB));
        GLIS_destroy_GLIS(G);
        LOG_INFO("Destroyed sub Compositor GLIS");
        LOG_INFO("Cleaned up");
    }
    return 0;
}