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

void rect() {
        // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions           // colors           // texture coords
            1.0f,   1.0f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            1.0f,  -1.0f, 0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
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

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void rectAlpha() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions           // colors                 // texture coords
             1.0f,  1.0f, 0.0f,    1.0f, 0.0f, 0.0f, 0.25f,   1.0f, 1.0f, // top right
             1.0f, -1.0f, 0.0f,    0.0f, 1.0f, 0.0f, 0.25f,   1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f, 1.0f, 0.25f,   0.0f, 0.0f, // bottom left
            -1.0f,  1.0f, 0.0f,    1.0f, 1.0f, 0.0f, 0.25f,   0.0f, 1.0f  // top left
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

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
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
    rectAlpha();
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
    glis.GLIS_build_simple_shader_program_RGBA(vertexShader, fragmentShader, shaderProgram);
    glUseProgram(shaderProgram);
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}
