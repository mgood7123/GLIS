#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <freetype/ftbbox.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLES3/gl32.h> // in ubuntu, this header is provided by libgles-dev
#include <glis/internal/internal.hpp>

class GLIS_FONT {
public:
    FT_Library GLIS_font;

    bool GLIS_font_init();

    FT_Face GLIS_font_face;

    bool GLIS_font_load(const char *font);

    void GLIS_font_set_size(int width, int height);

    struct Character {
        GLuint textureID;   // ID handle of the glyph texture
        glm::ivec2 size;    // Size of glyph
        glm::ivec2 bearing;  // Offset from baseline to left/top of glyph
        GLuint advance;    // Horizontal offset to advance to next glyph
    };

    std::map<GLchar, Character> sCharacters;

    bool GLIS_font_2D_store_ascii();

    void GLIS_font_free();

    GLuint GLIS_FONT_SHADER_PROGRAM;
    GLuint GLIS_FONT_VERTEX_SHADER;
    GLuint GLIS_FONT_FRAGMENT_SHADER;
    GLuint GLIS_FONT_VAO;
    GLuint GLIS_FONT_VBO;

    bool GLIS_font_init_shaders();

    bool GLIS_load_font(const char *font, int width, int height);

    void GLIS_font_RenderText(GLfloat w, GLfloat h, std::string text, float x, float y, float scale,
                              glm::vec3 color);

    GLfloat GLIS_font_w = 0;
    GLfloat GLIS_font_h = 0;

    void GLIS_font_set_RenderText_w_h(int w, int h);

    void GLIS_font_RenderText(std::string text, int x, int y, glm::vec3 color);

    glm::vec3 GLIS_font_color_black = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 GLIS_font_color_white = glm::vec3(1.0f, 1.0f, 1.0f);
};