//
// Created by smallville7123 on 14/07/20.
//

//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

const char *vertex_shader_source_RGB = R"glsl( #version 300 es
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

const char *fragment_shader_source_RGB = R"glsl( #version 300 es
    out highp vec4 FragColor;
    in highp vec3 ourColor;
    void main()
    {
        FragColor = vec4(ourColor, 1.0);
    }
)glsl";

const char *texture_vertex_shader_source_RGB = R"glsl( #version 300 es
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

const char *texture_fragment_shader_source_RGB = R"glsl( #version 300 es
    out highp vec4 FragColor;
    in highp vec4 ourColor;
    in highp vec2 TexCoord;
    uniform sampler2D texture1;
    void main()
    {
        FragColor = texture(texture1, TexCoord);
    }
)glsl";

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

GLuint vertexShader2;
GLuint fragmentShader2;
GLuint shaderProgram2;

GLuint framebuffer;
GLuint texture;
GLuint texture2;
GLuint rbo;

void rectAlpha(
    std::initializer_list<float> cornerTopLeft,
    std::initializer_list<float> cornerBottomRight,
    std::initializer_list<float> textureColorTopRight,
    std::initializer_list<float> shaderColorTopRight,
    std::initializer_list<float> textureCoordinatesTopRight,
    std::initializer_list<float> textureColorBottomRight,
    std::initializer_list<float> shaderColorBottomRight,
    std::initializer_list<float> textureCoordinatesBottomRight,
    std::initializer_list<float> textureColorBottomLeft,
    std::initializer_list<float> shaderColorBottomLeft,
    std::initializer_list<float> textureCoordinatesBottomLeft,
    std::initializer_list<float> textureColorTopLeft,
    std::initializer_list<float> shaderColorTopLeft,
    std::initializer_list<float> textureCoordinatesTopLeft
) {
    const float * cornerTopLeft_ = cornerTopLeft.begin();
    const float * cornerBottomRight_ = cornerBottomRight.begin();

    const float * textureColorTopRight_ = textureColorTopRight.begin();
    const float * shaderColorTopRight_ = shaderColorTopRight.begin();
    const float * textureCoordinatesTopRight_ = textureCoordinatesTopRight.begin();

    const float * textureColorBottomRight_ = textureColorBottomRight.begin();
    const float * shaderColorBottomRight_ = shaderColorBottomRight.begin();
    const float * textureCoordinatesBottomRight_ = textureCoordinatesBottomRight.begin();

    const float * textureColorBottomLeft_ = textureColorBottomLeft.begin();
    const float * shaderColorBottomLeft_ = shaderColorBottomLeft.begin();
    const float * textureCoordinatesBottomLeft_ = textureCoordinatesBottomLeft.begin();

    const float * textureColorTopLeft_ = textureColorTopLeft.begin();
    const float * shaderColorTopLeft_ = shaderColorTopLeft.begin();
    const float * textureCoordinatesTopLeft_ = textureCoordinatesTopLeft.begin();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // https://learnopengl.com/img/getting-started/ndc.png
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // top right
            cornerBottomRight_[0],
            cornerTopLeft_[1],
            textureColorTopRight_[0],
            textureColorTopRight_[1],
            textureColorTopRight_[2],
//             textureColorTopRight_[3],
//             shaderColorTopRight_[0],
//             shaderColorTopRight_[1],
//             shaderColorTopRight_[2],
//             shaderColorTopRight_[3],
            textureCoordinatesTopRight_[0],
            textureCoordinatesTopRight_[1],
            
            // bottom right
            cornerBottomRight_[0],
            cornerBottomRight_[1],
            textureColorBottomRight_[0],
            textureColorBottomRight_[1],
            textureColorBottomRight_[2],
//             textureColorBottomRight_[3],
//             shaderColorBottomRight_[0],
//             shaderColorBottomRight_[1],
//             shaderColorBottomRight_[2],
//             shaderColorBottomRight_[3],
            textureCoordinatesBottomLeft_[0],
            textureCoordinatesBottomLeft_[1],
            
            // bottom left
            cornerTopLeft_[0],
            cornerBottomRight_[1],
            textureColorBottomLeft_[0],
            textureColorBottomLeft_[1],
            textureColorBottomLeft_[2],
//             textureColorBottomLeft_[3],
//             shaderColorBottomLeft_[0],
//             shaderColorBottomLeft_[1],
//             shaderColorBottomLeft_[2],
//             shaderColorBottomLeft_[3],
            textureCoordinatesBottomLeft_[0],
            textureCoordinatesBottomLeft_[1],
            
            // top left
            cornerTopLeft_[0],
            cornerTopLeft_[1],
            textureColorTopLeft_[0],
            textureColorTopLeft_[1],
            textureColorTopLeft_[2],
//             textureColorTopLeft_[3],
//             shaderColorTopLeft_[0],
//             shaderColorTopLeft_[1],
//             shaderColorTopLeft_[2],
//             shaderColorTopLeft_[3],
            textureCoordinatesTopLeft_[1],
            textureCoordinatesTopLeft_[1]
    };

    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    const int position_size = 2;
    const int texture_size = 3;
    const int shader_size = 0;
    const int coordinate_size = 2;
    const int vertex_size = (position_size+texture_size+shader_size+coordinate_size)*sizeof(float);
    
    const void * position_index = (void*)(0);
    const void * texture_index = (void*)(sizeof(float) * (position_size));
    const void * shader_index = (void*)(sizeof(float) * (position_size+texture_size));
    const void * coordinate_index = (void*)(sizeof(float) * (position_size+texture_size+shader_size));

    glVertexAttribPointer(0, position_size, GL_FLOAT, GL_FALSE, vertex_size, position_index);
    glVertexAttribPointer(1, texture_size, GL_FLOAT, GL_FALSE, vertex_size, texture_index);
//     glVertexAttribPointer(2, shader_size, GL_FLOAT, GL_FALSE, vertex_size, shader_index);
    glVertexAttribPointer(2, coordinate_size, GL_FLOAT, GL_FALSE, vertex_size, coordinate_index);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
//     glEnableVertexAttribArray(3);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDisable(GL_BLEND);
}

const GLint W = 400;
const GLint H = 400;

GLuint textureData[W*H];

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    // clear to black
    glis.clearBlack();

    // render at full screen and rescale
    
    // draw a texture

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); glis.GLIS_error_to_string_GL("glBindFramebuffer");
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, texture); glis.GLIS_error_to_string_GL("glBindTexture");
    
    rectAlpha(
        { -1.0f,  1.0f}, // cornerTopLeft
        {  1.0f, -1.0f}, // cornerBottomRight
        {1.0f, 0.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {1.0f, 1.0f}, // texture coordinates
        {0.0f, 1.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {1.0f, 0.0f}, // texture coordinates
        {0.0f, 0.0f, 1.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {0.0f, 0.0f}, // texture coordinates
        {1.0f, 1.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {0.0f, 1.0f} // texture coordinates
    );

    glReadPixels(0, 0, W,H, GL_RGBA, GL_UNSIGNED_BYTE, textureData); glis.GLIS_error_to_string_GL("glReadPixels");
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); glis.GLIS_error_to_string_GL("glBindFramebuffer");
    
    glUseProgram(shaderProgram2);

    if (texture2 == 0) {
        glGenTextures(1, &texture2); glis.GLIS_error_to_string_GL("glGenTextures");
        glBindTexture(GL_TEXTURE_2D, texture2); glis.GLIS_error_to_string_GL("glBindTexture");
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, W,H); glis.GLIS_error_to_string_GL("glTexStorage2D");
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W,H, GL_RGBA, GL_UNSIGNED_BYTE, textureData); glis.GLIS_error_to_string_GL("glTexSubImage2D");
        glGenerateMipmap(GL_TEXTURE_2D); glis.GLIS_error_to_string_GL("glGenerateMipmap");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); glis.GLIS_error_to_string_GL("glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); glis.GLIS_error_to_string_GL("glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); glis.GLIS_error_to_string_GL("glTexParameteri");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); glis.GLIS_error_to_string_GL("glTexParameteri");
    } else {
        glBindTexture(GL_TEXTURE_2D, texture2); glis.GLIS_error_to_string_GL("glBindTexture");
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W,H, GL_RGBA, GL_UNSIGNED_BYTE, textureData); glis.GLIS_error_to_string_GL("glTexSubImage2D");
    }
    
    rectAlpha(
        { -1.0f,  1.0f}, // cornerTopLeft
        {  1.0f, -1.0f}, // cornerBottomRight
        {1.0f, 0.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {1.0f, 1.0f}, // texture coordinates
        {0.0f, 1.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {1.0f, 0.0f}, // texture coordinates
        {0.0f, 0.0f, 1.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {0.0f, 0.0f}, // texture coordinates
        {1.0f, 1.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {0.0f, 1.0f} // texture coordinates
    );
    
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glis.GLIS_Viewport(renderer);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glDeleteProgram(shaderProgram2);
    glDeleteShader(fragmentShader2);
    glDeleteShader(vertexShader2);
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
    glis.GLIS_texture_buffer(framebuffer, rbo, texture, W,H);
    glis.GLIS_build_simple_shader_program(
        vertexShader, vertex_shader_source_RGB,
        fragmentShader, fragment_shader_source_RGB,
        shaderProgram
    );
    glis.GLIS_build_simple_shader_program(
        vertexShader2, texture_vertex_shader_source_RGB,
        fragmentShader2, texture_fragment_shader_source_RGB,
        shaderProgram2
    );
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
