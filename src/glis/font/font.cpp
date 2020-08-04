#include <glis/font/font.hpp>
#include <glis/internal/log.hpp>
#include <glis/glis.hpp>
#include <utility>

bool useAtlas = true;

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
    this->width = width;
    this->height = height;
    FT_Set_Pixel_Sizes(GLIS_font_face, width, height);
}

bool GLIS_FONT::GLIS_font_2D_store_ascii_atlas() {
    int MAXWIDTH = 1024;
    unsigned int roww = 0;
    unsigned int rowh = 0;
    texture_atlas.w = 0;
    texture_atlas.h = 0;

    memset(texture_atlas.c, 0, sizeof texture_atlas.c);

    /* Find minimum size for a texture holding all visible ASCII characters */
    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(GLIS_font_face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        if (roww + GLIS_font_face->glyph->bitmap.width + 1 >= MAXWIDTH) {
            texture_atlas.w = std::max(texture_atlas.w, roww);
            texture_atlas.h += rowh;
            roww = 0;
            rowh = 0;
        }
        roww += GLIS_font_face->glyph->bitmap.width + 1;
        rowh = std::max(rowh, GLIS_font_face->glyph->bitmap.rows);
    }

    texture_atlas.w = std::max(texture_atlas.w, roww);
    texture_atlas.h += rowh;

    /* Create a texture that will be used to hold all ASCII glyphs */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture_atlas.tex);
    glBindTexture(GL_TEXTURE_2D, texture_atlas.tex);
    glUniform1i(uniform_tex, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texture_atlas.w, texture_atlas.h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);

//    glTexImage2D(
//            GL_TEXTURE_2D,
//            0,
//            GL_R8,
//            texture_atlas.w,
//            texture_atlas.h,
//            0,
//            GL_RED,
//            GL_UNSIGNED_BYTE,
//            0
//    );

    /* We require 1 byte alignment when uploading texture data */
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Paste all glyph bitmaps into the texture, remembering the offset */
    int ox = 0;
    int oy = 0;

    rowh = 0;

    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(GLIS_font_face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }

        if (ox + GLIS_font_face->glyph->bitmap.width + 1 >= MAXWIDTH) {
            oy += rowh;
            rowh = 0;
            ox = 0;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, GLIS_font_face->glyph->bitmap.width, GLIS_font_face->glyph->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, GLIS_font_face->glyph->bitmap.buffer);
        texture_atlas.c[i].ax = GLIS_font_face->glyph->advance.x >> 6;
        texture_atlas.c[i].ay = GLIS_font_face->glyph->advance.y >> 6;

        texture_atlas.c[i].bw = GLIS_font_face->glyph->bitmap.width;
        texture_atlas.c[i].bh = GLIS_font_face->glyph->bitmap.rows;

        texture_atlas.c[i].bl = GLIS_font_face->glyph->bitmap_left;
        texture_atlas.c[i].bt = GLIS_font_face->glyph->bitmap_top;

        texture_atlas.c[i].tx = ox / (float)texture_atlas.w;
        texture_atlas.c[i].ty = oy / (float)texture_atlas.h;

        rowh = std::max(rowh, GLIS_font_face->glyph->bitmap.rows);
        ox += GLIS_font_face->glyph->bitmap.width + 1;
    }

    LOG_INFO("Generated a %d x %d (%d kb) texture atlas", texture_atlas.w, texture_atlas.h, texture_atlas.w * texture_atlas.h / 1024);

    // reset unpack alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
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

bool GLIS_FONT::GLIS_font_init_shaders() {
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
    GLIS glis;
    glis.GLIS_build_simple_shader_program(
            GLIS_FONT_VERTEX_SHADER, vs, GLIS_FONT_FRAGMENT_SHADER, fs, GLIS_FONT_SHADER_PROGRAM
    );
    glUseProgram(GLIS_FONT_SHADER_PROGRAM);
    uniform_projection = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "projection");
    uniform_color = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "textColor");
    uniform_tex = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "text");
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &GLIS_FONT_VAO);
    glGenBuffers(1, &GLIS_FONT_VBO);
    glBindVertexArray(GLIS_FONT_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, GLIS_FONT_VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return true;
}

bool GLIS_FONT::GLIS_load_font(const char *font, int width, int height) {
    // TODO: smart load
    if (!GLIS_font_init_shaders()) return false;
    if (!GLIS_font_init()) return false;
    if (!GLIS_font_load(font)) {
        FT_Done_FreeType(GLIS_font);
        return false;
    }
    GLIS_font_set_size(width, height);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    bool r = GLIS_font_2D_store_ascii();
    bool r2 = GLIS_font_2D_store_ascii_atlas();

    // TODO: restore alignment from paramater

    // enable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    if (!r && !r2) {
        GLIS_font_free();
        return false;
    }
    return true;
}

void
GLIS_FONT::GLIS_font_RenderText(int w, int h, std::string text, int x, int y,
                                unsigned int scale, glm::vec3 color) {
    if (useAtlas) {
        const uint8_t *p;
        point coords[6 * text.length()];
        int c = 0;

        /* Loop through all characters */
        for (p = (const uint8_t *)text.c_str(); *p; p++) {
            /* Calculate the vertex and texture coordinates */
            float x2 = x + texture_atlas.c[*p].bl * static_cast<float>(scale);
            float y2 = -y - texture_atlas.c[*p].bt * static_cast<float>(scale);
            float w = texture_atlas.c[*p].bw * static_cast<float>(scale);
            float h = texture_atlas.c[*p].bh * static_cast<float>(scale);

            /* Advance the cursor to the start of the next character */
            x += texture_atlas.c[*p].ax * static_cast<float>(scale);
            y += texture_atlas.c[*p].ay * static_cast<float>(scale);

            /* Skip glyphs that have no pixels */
            if (!w || !h)
                continue;

            coords[c++] = (point) {
                    x2, -y2, texture_atlas.c[*p].tx, texture_atlas.c[*p].ty};
            coords[c++] = (point) {
                    x2 + w, -y2, texture_atlas.c[*p].tx + texture_atlas.c[*p].bw / texture_atlas.w, texture_atlas.c[*p].ty};
            coords[c++] = (point) {
                    x2, -y2 - h, texture_atlas.c[*p].tx, texture_atlas.c[*p].ty + texture_atlas.c[*p].bh / texture_atlas.h};
            coords[c++] = (point) {
                    x2 + w, -y2, texture_atlas.c[*p].tx + texture_atlas.c[*p].bw / texture_atlas.w, texture_atlas.c[*p].ty};
            coords[c++] = (point) {
                    x2, -y2 - h, texture_atlas.c[*p].tx, texture_atlas.c[*p].ty + texture_atlas.c[*p].bh / texture_atlas.h};
            coords[c++] = (point) {
                    x2 + w, -y2 - h, texture_atlas.c[*p].tx + texture_atlas.c[*p].bw / texture_atlas.w, texture_atlas.c[*p].ty + texture_atlas.c[*p].bh / texture_atlas.h};
        }

        /* Draw all the character on the screen in one go */
        glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, c);
    } else {
        // flip y axis
        GLfloat y_ = static_cast<float>(h-y);
        // Iterate through all characters
        std::string::const_iterator c;

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
                float y__1 = static_cast<float>(ch.size.y - ch.bearing.y);
                float y__2 = static_cast<float>(yOffset);
                GLfloat ypos = y_ - y__1 - y__2;

                // apply scaling
                xpos *= static_cast<float>(scale);
                ypos *= static_cast<float>(scale);

                GLfloat w_ = ch.size.x * static_cast<float>(scale);
                GLfloat h_ = ch.size.y * static_cast<float>(scale);

                // Update VBO for each character
                // positions are not device normalized coordinates

                GLfloat vertices[6][4] = {
                        {xpos,      ypos + h_, 0.0f, 0.0f},
                        {xpos,      ypos,      0.0f, 1.0f},
                        {xpos + w_, ypos,      1.0f, 1.0f},

                        {xpos,      ypos + h_, 0.0f, 0.0f},
                        {xpos + w_, ypos,      1.0f, 1.0f},
                        {xpos + w_, ypos + h_, 1.0f, 0.0f}
                };

                // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
                // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
                int advanced = static_cast<int>((ch.advance >> static_cast<unsigned int>(6)) *
                                                scale);
                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.textureID);
                // Be sure to use glBufferSubData and not glBufferData
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                x += advanced;
            }
        }
    }
}

void GLIS_FONT::GLIS_font_set_RenderText_w_h(int w, int h) {
    max_width = w;
    max_height = h;
}

void GLIS_FONT::GLIS_font_RenderText(std::string text, int x, int y, glm::vec3 color) {
    GLIS_font_RenderText(max_width, max_height, std::move(text), x, y, 1.0f, color);
}

void GLIS_FONT::GLIS_font_RenderTextDNC(std::string text, float DNC_x, float DNC_y, glm::vec3 color) {
    GLIS_font_RenderTextDNC(max_width, max_width, std::move(text), DNC_x, DNC_y, 1.0f, color);
}

void
GLIS_FONT::GLIS_font_RenderTextDNC(int w, int h, std::string text, float DNC_x, float DNC_y,
                                float scale, glm::vec3 color) {
//    GLIS_BACKUP backup;
//    backup.backup();
//
//    // Using Shader program
//    glUseProgram(GLIS_FONT_SHADER_PROGRAM);
//
//    // do something
//
//    glm::mat4 projection = glm::ortho(0.0f, w, 0.0f, h);
//
//    GLuint loc = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "projection");
//    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
//
//    loc = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "textColor");
//    glUniform3f(loc, color.x, color.y, color.z);
//
//    // culling improves performance
//    glEnable(GL_CULL_FACE);
//
//    // blend into background
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    glActiveTexture(GL_TEXTURE0);
//    glBindVertexArray(GLIS_FONT_VAO);
//
//    // Iterate through all characters
//    std::string::const_iterator c;
//
//    for (c = text.begin(); c != text.end(); c++) {
//
//        if (sCharacters.count(*c) > 0) {
//
//            Character ch = sCharacters[*c];
//
//            // set to origin
//            GLfloat xpos = ch.bearing.x;
//            GLfloat ypos = ch.size.y - ch.bearing.y;
//
//            // apply scaling
//            xpos *= scale;
//            ypos *= scale;
//
//            GLfloat w_ = ch.size.x * scale;
//            GLfloat h_ = ch.size.y * scale;
//
//            // Update VBO for each character
//            GLfloat vertices[6][4] = {
//                    {xpos,      ypos + h_, 0.0f, 0.0f},
//                    {xpos,      ypos,      0.0f, 1.0f},
//                    {xpos + w_, ypos,      1.0f, 1.0f},
//
//                    {xpos,      ypos + h_, 0.0f, 0.0f},
//                    {xpos + w_, ypos,      1.0f, 1.0f},
//                    {xpos + w_, ypos + h_, 1.0f, 0.0f}
//            };
//            // Render glyph texture over quad
//            glBindTexture(GL_TEXTURE_2D, ch.textureID);
//            // Update content of VBO memory
//            glBindBuffer(GL_ARRAY_BUFFER, GLIS_FONT_VBO);
//            // Be sure to use glBufferSubData and not glBufferData
//            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//
//            glBindBuffer(GL_ARRAY_BUFFER, 0);
//            // Render quad
//            glDrawArrays(GL_TRIANGLES, 0, 6);
//            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//            // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//            x += (ch.advance >> 6) * scale;
//        }
//    }
//    glDisable(GL_BLEND);
//    glDisable(GL_CULL_FACE);
//    backup.restore();
}

GLIS_FONT::ATLAS::atlas::atlas(FT_Face face, int height) {
    FT_Set_Pixel_Sizes(face, 0, height);
    FT_GlyphSlot g = face->glyph;

    unsigned int roww = 0;
    unsigned int rowh = 0;
    w = 0;
    h = 0;

    memset(c, 0, sizeof c);

    /* Find minimum size for a texture holding all visible ASCII characters */
    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        if (roww + g->bitmap.width + 1 >= MAXWIDTH) {
            w = std::max(w, roww);
            h += rowh;
            roww = 0;
            rowh = 0;
        }
        roww += g->bitmap.width + 1;
        rowh = std::max(rowh, g->bitmap.rows);
    }

    w = std::max(w, roww);
    h += rowh;

    /* Create a texture that will be used to hold all ASCII glyphs */
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Paste all glyph bitmaps into the texture, remembering the offset */
    int ox = 0;
    int oy = 0;

    rowh = 0;

    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }

        if (ox + g->bitmap.width + 1 >= MAXWIDTH) {
            oy += rowh;
            rowh = 0;
            ox = 0;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        c[i].ax = g->advance.x >> 6;
        c[i].ay = g->advance.y >> 6;

        c[i].bw = g->bitmap.width;
        c[i].bh = g->bitmap.rows;

        c[i].bl = g->bitmap_left;
        c[i].bt = g->bitmap_top;

        c[i].tx = ox / (float)w;
        c[i].ty = oy / (float)h;

        rowh = std::max(rowh, g->bitmap.rows);
        ox += g->bitmap.width + 1;
    }

    fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
}

GLIS_FONT::ATLAS::atlas::~atlas() {
    glDeleteTextures(1, &tex);
}

void GLIS_FONT::ATLAS::render_text(const char *text, GLIS_FONT & font, GLIS_FONT::ATLAS::atlas *a, float x, float y,
                                   float sx, float sy) {
    printf("text: %s\n", text);
    printf("x: %G, y: %G\n", x, y);
    printf("sx: %G, sy: %G\n", 1.0f, 1.0f);
    const uint8_t *p;

    /* Use the texture containing the atlas */
    glBindTexture(GL_TEXTURE_2D, a->tex);
    glUniform1i(uniform_tex, 0);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, static_cast<GLfloat>(font.width), 0.0f, static_cast<GLfloat>(font.height))));

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    point coords[6 * strlen(text)];
    int c = 0;

    /* Loop through all characters */
    for (p = (const uint8_t *)text; *p; p++) {
        /* Calculate the vertex and texture coordinates */
        float x2 = x + a->c[*p].bl * sx;
        float y2 = -y - a->c[*p].bt * sy;
        float w = a->c[*p].bw * sx;
        float h = a->c[*p].bh * sy;

        /* Advance the cursor to the start of the next character */
        x += a->c[*p].ax * sx;
        y += a->c[*p].ay * sy;

        /* Skip glyphs that have no pixels */
        if (!w || !h)
            continue;

        coords[c++] = (point) {
                x2, -y2, a->c[*p].tx, a->c[*p].ty};
        coords[c++] = (point) {
                x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty};
        coords[c++] = (point) {
                x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h};
        coords[c++] = (point) {
                x2 + w, -y2, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty};
        coords[c++] = (point) {
                x2, -y2 - h, a->c[*p].tx, a->c[*p].ty + a->c[*p].bh / a->h};
        coords[c++] = (point) {
                x2 + w, -y2 - h, a->c[*p].tx + a->c[*p].bw / a->w, a->c[*p].ty + a->c[*p].bh / a->h};
    }

    /* Draw all the character on the screen in one go */
    glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, c);

    glDisableVertexAttribArray(attribute_coord);
}

void GLIS_FONT::ATLAS::display(GLIS_FONT & font) {
    glUseProgram(program);

    /* White background */
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Enable blending, necessary for our alpha texture */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat black[4] = { 0, 0, 0, 1 };
    GLfloat red[4] = { 1, 0, 0, 1 };
    GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };

    /* Set color to black */
    glUniform4fv(uniform_color, 1, black);

    /* Effects of alignment */
    render_text("The Quick Brown Fox Jumps Over The Lazy Dog", font, a48, 8, 50, 1.0f, 1.0f);
    render_text("The Misaligned Fox Jumps Over The Lazy Dog", font, a48, 8.5, 100.5, 1.0f, 1.0f);

    /* Scaling the texture versus changing the font size */
    render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", font, a48, 8, 175, 0.5f, 0.5f);
    render_text("The Small Font Sized Fox Jumps Over The Lazy Dog", font, a24, 8, 200, 1.0f, 1.0f);
    render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", font, a48, 8, 235, 0.25, 0.25);
    render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog", font, a12, 8, 250, 1.0f, 1.0f);

    /* Colors and transparency */
    render_text("The Solid Black Fox Jumps Over The Lazy Dog", font, a48, 8, 430, 1.0f, 1.0f);

    glUniform4fv(uniform_color, 1, red);
    render_text("The Solid Red Fox Jumps Over The Lazy Dog", font, a48, 8, 330, 1.0f, 1.0f);
    render_text("The Solid Red Fox Jumps Over The Lazy Dog", font, a48, 28, 450, 1.0f, 1.0f);

    glUniform4fv(uniform_color, 1, transparent_green);
    render_text("The Transparent Green Fox Jumps Over The Lazy Dog", font, a48, 0, 380, 1.0f, 1.0f);
    render_text("The Transparent Green Fox Jumps Over The Lazy Dog", font, a48, 18, 440, 1.0f, 1.0f);
}

void GLIS_FONT::ATLAS::free_resources() {
    glDeleteProgram(program);
    glDeleteShader(f);
    glDeleteShader(v);
}

GLIS_FONT::ATLAS::ATLAS() {
    /* Initialize the FreeType2 library */
    if (FT_Init_FreeType(&ft)) {
        LOG_ERROR("Could not init freetype library");
    }
}

void GLIS_FONT::ATLAS::set_width_height(int w, int h) {
    width = w;
    height = h;
}

int GLIS_FONT::ATLAS::load_font(const char * font_name, const char * font_path) {
//    if (atlas_index.at(font_name))
    /* Load a font */
    if (FT_New_Face(ft, fontfilename, 0, &face)) {
        LOG_ERROR("Could not open font %s", fontfilename);
        return 0;
    }

    // a font has been successfully loaded, store it in the atlas map
//    atlas_index.insert({font_name, nullptr});

    GLIS g;
    g.GLIS_build_simple_shader_program(
            v,
            R"glsl( #version 300 es
        layout (location = 0) in vec4 coord;
        out vec2 texpos;
        uniform mat4 projection;

        void main(void) {
            gl_Position = projection * vec4(coord.xy, 0, 1);
            texpos = coord.zw;
        }
    )glsl",
            f,
            R"glsl( #version 300 es
        precision mediump float;

        in vec2 texpos;
        uniform sampler2D tex;
        uniform vec4 color;
        out vec4 c;

        void main(void) {
            c = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
        }
    )glsl",
            program
    );
    if(program == 0)
        return 0;

    GLIS::GLIS_error_to_string_GL("glGet");
    attribute_coord = glGetAttribLocation(program, "coord");
    uniform_tex = glGetUniformLocation(program, "tex");
    uniform_color = glGetUniformLocation(program, "color");

    if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
        return 0;

    // Create the vertex buffer object
    glGenBuffers(1, &vbo);
    return 1;
}

void GLIS_FONT::ATLAS::generate_font_size() {

}
