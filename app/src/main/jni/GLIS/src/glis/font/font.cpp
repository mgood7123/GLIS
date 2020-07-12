#include <glis/font/font.hpp>
#include <glis/internal/log.hpp>
#include <glis/glis.hpp>

bool GLIS_FONT::GLIS_font_init() {
    if (FT_Init_FreeType(&GLIS_font)) {
        LOG_ERROR("ERROR::FREETYPE: Could not init FreeType Library");
        return false;
    }
    return true;
}

bool GLIS_FONT::GLIS_font_load(const char *font) {
    if (FT_New_Face(GLIS_font, font, 0, &GLIS_font_face)) {
        LOG_ERROR("ERROR::FREETYPE: Failed to load font");
        return false;
    }
    return true;
}

void GLIS_FONT::GLIS_font_set_size(int width, int height) {
    FT_Set_Pixel_Sizes(GLIS_font_face, width, height);
}

bool GLIS_FONT::GLIS_font_2D_store_ascii() {
    // Load first 128 characters of ASCII set
    int loaded = 0;
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(GLIS_font_face, c, FT_LOAD_RENDER)) {
            LOG_ERROR("ERROR::FREETYTPE: Failed to load Glyph");
            continue;
        }
        loaded++;
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_R8,
                GLIS_font_face->glyph->bitmap.width,
                GLIS_font_face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                GLIS_font_face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(GLIS_font_face->glyph->bitmap.width, GLIS_font_face->glyph->bitmap.rows),
                glm::ivec2(GLIS_font_face->glyph->bitmap_left, GLIS_font_face->glyph->bitmap_top),
                (GLuint) GLIS_font_face->glyph->advance.x
        };
        sCharacters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return loaded != 0;
}

void GLIS_FONT::GLIS_font_free() {
    FT_Done_Face(GLIS_font_face);
    FT_Done_FreeType(GLIS_font);
}

bool GLIS_FONT::GLIS_font_init_shaders(class GLIS & glis) {
    const char *vs = R"glsl( #version 300 es
    layout (location = 0) in vec4 vertex;
    out vec2 TexCoords;

    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        TexCoords = vertex.zw;
    }
    )glsl";

    const char *fs = R"glsl( #version 300 es
    precision mediump float;

    in vec2 TexCoords;
    out vec4 color;

    uniform sampler2D text;
    uniform vec3 textColor;

    void main()
    {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
        color = vec4(textColor, 1.0) * sampled;
    }
    )glsl";

    GLIS_FONT_VERTEX_SHADER = glis.GLIS_createShader(GL_VERTEX_SHADER, vs);
    GLIS_FONT_FRAGMENT_SHADER = glis.GLIS_createShader(GL_FRAGMENT_SHADER, fs);
    LOG_INFO("Creating Shader program");
    GLIS_FONT_SHADER_PROGRAM = glCreateProgram();
    LOG_INFO("Attaching vertex Shader to program");
    glAttachShader(GLIS_FONT_SHADER_PROGRAM, GLIS_FONT_VERTEX_SHADER);
    LOG_INFO("Attaching fragment Shader to program");
    glAttachShader(GLIS_FONT_SHADER_PROGRAM, GLIS_FONT_FRAGMENT_SHADER);
    LOG_INFO("Linking Shader program");
    glLinkProgram(GLIS_FONT_SHADER_PROGRAM);
    LOG_INFO("Validating Shader program");
    GLboolean ProgramIsValid = glis.GLIS_validate_program(GLIS_FONT_SHADER_PROGRAM);
    assert(ProgramIsValid == GL_TRUE);
    LOG_INFO("Shader program is valid");
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &GLIS_FONT_VAO);
    glGenBuffers(1, &GLIS_FONT_VBO);
    glBindVertexArray(GLIS_FONT_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, GLIS_FONT_VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}

bool GLIS_FONT::GLIS_load_font(GLIS & glis, const char *font, int width, int height) {
    // TODO: smart load
    if (!GLIS_font_init_shaders(glis)) return false;
    if (!GLIS_font_init()) return false;
    if (!GLIS_font_load(font)) {
        FT_Done_FreeType(GLIS_font);
        return false;
    }
    GLIS_font_set_size(width, height);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    bool r = GLIS_font_2D_store_ascii();

    // TODO: restore alignment from paramater

    // enable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (!r) {
        GLIS_font_free();
        return false;
    }
    return true;
}

void
GLIS_FONT::GLIS_font_RenderText(GLfloat w, GLfloat h, std::string text, float x, float y, float scale,
                           glm::vec3 color) {
    GLIS_BACKUP backup;
    backup.backup();

    // Using Shader program
    glUseProgram(GLIS_FONT_SHADER_PROGRAM);

    // do something

    glm::mat4 projection = glm::ortho(0.0f, w, 0.0f, h);

    GLuint loc = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "projection");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));

    loc = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "textColor");
    glUniform3f(loc, color.x, color.y, color.z);

    // culling improves performance
    glEnable(GL_CULL_FACE);

    // blend into background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(GLIS_FONT_VAO);

    // Iterate through all characters
    std::string::const_iterator c;
    GLfloat y_ = y - h;
    y_ = y_ < 0 ? -y_ : y_;

    // first we calculate the maxY of each character we are going to be rendering

    int topOffset = 0;
    int bottomOffset = 0;

    for (c = text.begin(); c != text.end(); c++)
        if (sCharacters.count(*c) > 0) {
            Character ch = sCharacters[*c];
            if (ch.bearing.y > topOffset) topOffset = ch.bearing.y;
            if ((ch.size.y - ch.bearing.y) > bottomOffset)
                bottomOffset = (ch.size.y - ch.bearing.y);
        }

    // clamp y to top and bottom offsets

    int yOffset = 0;
    if (y < topOffset) {
        yOffset = topOffset;
    } else {
        int bottom = h - bottomOffset;
        if (y > bottom) {
            yOffset = -(bottomOffset);
        }
    }

    // then we apply this to the entire string

    for (c = text.begin(); c != text.end(); c++) {

        if (sCharacters.count(*c) > 0) {

            Character ch = sCharacters[*c];

            // set to origin
            GLfloat xpos = x + ch.bearing.x;
            GLfloat ypos = y_ - (ch.size.y - ch.bearing.y) - yOffset;

            // apply scaling
            xpos *= scale;
            ypos *= scale;

            GLfloat w_ = ch.size.x * scale;
            GLfloat h_ = ch.size.y * scale;

            // Update VBO for each character
            GLfloat vertices[6][4] = {
                    {xpos,      ypos + h_, 0.0f, 0.0f},
                    {xpos,      ypos,      0.0f, 1.0f},
                    {xpos + w_, ypos,      1.0f, 1.0f},

                    {xpos,      ypos + h_, 0.0f, 0.0f},
                    {xpos + w_, ypos,      1.0f, 1.0f},
                    {xpos + w_, ypos + h_, 1.0f, 0.0f}
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, GLIS_FONT_VBO);
            // Be sure to use glBufferSubData and not glBufferData
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
            x += (ch.advance >> 6) * scale;
        }
    }
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    backup.restore();
}

void GLIS_FONT::GLIS_font_set_RenderText_w_h(int w, int h) {
    GLIS_font_w = w;
    GLIS_font_h = h;
}

void GLIS_FONT::GLIS_font_RenderText(std::string text, int x, int y, glm::vec3 color) {

    GLIS_font_RenderText(GLIS_font_w, GLIS_font_h, text, x, y, 1.0f, color);
}