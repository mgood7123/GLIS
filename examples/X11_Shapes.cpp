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
GLuint texture;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

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
            textureColorTopRight_[3],
            shaderColorTopRight_[0],
            shaderColorTopRight_[1],
            shaderColorTopRight_[2],
            shaderColorTopRight_[3],
            textureCoordinatesTopRight_[0],
            textureCoordinatesTopRight_[1],
            
            // bottom right
            cornerBottomRight_[0],
            cornerBottomRight_[1],
            textureColorBottomRight_[0],
            textureColorBottomRight_[1],
            textureColorBottomRight_[2],
            textureColorBottomRight_[3],
            shaderColorBottomRight_[0],
            shaderColorBottomRight_[1],
            shaderColorBottomRight_[2],
            shaderColorBottomRight_[3],
            textureCoordinatesBottomLeft_[0],
            textureCoordinatesBottomLeft_[1],
            
            // bottom left
            cornerTopLeft_[0],
            cornerBottomRight_[1],
            textureColorBottomLeft_[0],
            textureColorBottomLeft_[1],
            textureColorBottomLeft_[2],
            textureColorBottomLeft_[3],
            shaderColorBottomLeft_[0],
            shaderColorBottomLeft_[1],
            shaderColorBottomLeft_[2],
            shaderColorBottomLeft_[3],
            textureCoordinatesBottomLeft_[0],
            textureCoordinatesBottomLeft_[1],
            
            // top left
            cornerTopLeft_[0],
            cornerTopLeft_[1],
            textureColorTopLeft_[0],
            textureColorTopLeft_[1],
            textureColorTopLeft_[2],
            textureColorTopLeft_[3],
            shaderColorTopLeft_[0],
            shaderColorTopLeft_[1],
            shaderColorTopLeft_[2],
            shaderColorTopLeft_[3],
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
    const int texture_size = 4;
    const int shader_size = 4;
    const int coordinate_size = 2;
    const int vertex_size = (position_size+texture_size+shader_size+coordinate_size)*sizeof(float);
    
    const void * position_index = (void*)(0);
    const void * texture_index = (void*)(sizeof(float) * (position_size));
    const void * shader_index = (void*)(sizeof(float) * (position_size+texture_size));
    const void * coordinate_index = (void*)(sizeof(float) * (position_size+texture_size+shader_size));

    glVertexAttribPointer(0, position_size, GL_FLOAT, GL_FALSE, vertex_size, position_index);
    glVertexAttribPointer(1, texture_size, GL_FLOAT, GL_FALSE, vertex_size, texture_index);
    glVertexAttribPointer(2, shader_size, GL_FLOAT, GL_FALSE, vertex_size, shader_index);
    glVertexAttribPointer(3, coordinate_size, GL_FLOAT, GL_FALSE, vertex_size, coordinate_index);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

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

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    // clear to black
    glis.clearBlack();
    
    rectAlpha(
        {-1.0f, 1.0f}, // cornerTopLeft
        {0.0f, -0.5f}, // cornerBottomRight
        
        // top right
        {1.0f, 0.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {1.0f, 1.0f}, // texture coordinates

        // bottom right
        {0.0f, 1.0f, 0.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {1.0f, 0.0f}, // texture coordinates

        // bottom left
        {0.0f, 0.0f, 1.0f, 1.0f}, // texture color
        {1.0f, 0.0f, 1.0f, 1.0f}, // shader color
        {0.0f, 0.0f}, // texture coordinates

        // top left
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
    glis.GLIS_build_simple_shader_program_RGBA(
        vertexShader,
        fragmentShader,
        shaderProgram
    );
    glUseProgram(shaderProgram);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
