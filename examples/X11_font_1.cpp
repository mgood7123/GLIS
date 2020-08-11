//
// Created by smallville7123 on 14/07/20.
//

#include <glis/glis.hpp>

GLIS_CLASS screen;
GLIS glis;
GLIS_FONT font;
GLIS_FPS fps;

const char *vertexSource = R"glsl( #version 300 es
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

const char *fragmentSource = R"glsl( #version 300 es
out highp vec4 FragColor;

in highp vec4 ourColor;
in highp vec2 TexCoord;

uniform sampler2D texture1;
//uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture1, TexCoord);
/*
    FragColor = mix(
        texture(texture1, TexCoord), // texture 1
        texture(texture2, TexCoord), // texture 2
        0.2 // interpolation,
        // If the third value is 0.0 it returns the first input
        // If it's 1.0 it returns the second input value.
        // A value of 0.2 will return 80% of the first input color and 20% of the second input color
        // resulting in a mixture of both our textures.
    );
*/
}
)glsl";

struct miniatlas {
    GLint width;
    GLint height;
    GLuint program;
    GLuint v;
    GLuint f;
    GLint attribute_coord;
    GLint uniform_tex;
    GLint uniform_color;

    struct point {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    };

    GLuint vbo;

    FT_Library ft;
    FT_Face face;

// Maximum texture width
#define MAXWIDTH 1024

    const char *fontfilename;

/**
 * The atlas struct holds a texture that contains the visible US-ASCII characters
 * of a certain font rendered with a certain character height.
 * It also contains an array that contains all the information necessary to
 * generate the appropriate vertex and texture coordinates for each character.
 *
 * After the constructor is run, you don't need to use any FreeType functions anymore.
 */
    struct atlas {
        GLuint tex;		// texture object

        unsigned int w;			// width of texture in pixels
        unsigned int h;			// height of texture in pixels

        struct {
            float ax;	// advance.x
            float ay;	// advance.y

            float bw;	// bitmap.width;
            float bh;	// bitmap.height;

            float bl;	// bitmap_left;
            float bt;	// bitmap_top;

            float tx;	// x offset of glyph in texture coordinates
            float ty;	// y offset of glyph in texture coordinates
        } c[128];		// character information

        atlas(FT_Face face, int height) {
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

        ~atlas() {
            glDeleteTextures(1, &tex);
        }
    };

    atlas *a48;
    atlas *a24;
    atlas *a12;

    int init_resources() {
        /* Initialize the FreeType2 library */
        if (FT_Init_FreeType(&ft)) {
            fprintf(stderr, "Could not init freetype library\n");
            return 0;
        }

        /* Load a font */
        if (FT_New_Face(ft, fontfilename, 0, &face)) {
            fprintf(stderr, "Could not open font %s\n", fontfilename);
            return 0;
        }

        const char *vs = R"glsl( #version 300 es
    layout (location = 0) in vec4 coord;
    out vec2 texpos;
    uniform mat4 projection;

    void main(void) {
        gl_Position = projection * vec4(coord.xy, 0, 1);
        texpos = coord.zw;
    }
    )glsl";

        const char *fs = R"glsl( #version 300 es
    precision mediump float;

    in vec2 texpos;
    uniform sampler2D tex;
    uniform vec4 color;
    out vec4 c;

    void main(void) {
        c = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
    }
    )glsl";

        glis.GLIS_build_simple_shader_program(
                v, vs,
                f, fs,
                program
        );
        if(program == 0)
            return 0;

        attribute_coord = glGetAttribLocation(program, "coord");
        uniform_tex = glGetUniformLocation(program, "tex");
        uniform_color = glGetUniformLocation(program, "color");

        if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
            return 0;

        // Create the vertex buffer object
        glGenBuffers(1, &vbo);

        /* Create texture atlasses for several font sizes */
        a48 = new atlas(face, 48);
        a24 = new atlas(face, 24);
        a12 = new atlas(face, 12);

        return 1;
    }

    /**
     * Render text using the currently loaded font and currently set font size.
     * Rendering starts at coordinates (x, y), z is always 0.
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas * a, float x, float y, float sx, float sy) {
        // flip y axis
        float y_ = static_cast<float>(height)-y;

        printf("text: %s\n", text);
        printf("x: %G, y: %G\n", x, y);
        printf("sx: %G, sy: %G\n", 1.0f, 1.0f);
        const uint8_t *p;

        /* Use the texture containing the atlas */
        glBindTexture(GL_TEXTURE_2D, a->tex);
        glUniform1i(uniform_tex, 0);
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height))));

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
            float y2 = -y_ - a->c[*p].bt * sy;
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

    void display() {
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
        render_text("The Quick Brown Fox Jumps Over The Lazy Dog", a48, 8, 50, 1.0f, 1.0f);
        render_text("The Misaligned Fox Jumps Over The Lazy Dog", a48, 8.5, 100.5, 1.0f, 1.0f);

        /* Scaling the texture versus changing the font size */
        render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", a48, 8, 175, 0.5f, 0.5f);
        render_text("The Small Font Sized Fox Jumps Over The Lazy Dog", a24, 8, 200, 1.0f, 1.0f);
        render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", a48, 8, 235, 0.25, 0.25);
        render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog", a12, 8, 250, 1.0f, 1.0f);

        /* Colors and transparency */
        render_text("The Solid Black Fox Jumps Over The Lazy Dog", a48, 8, 430, 1.0f, 1.0f);

        glUniform4fv(uniform_color, 1, red);
        render_text("The Solid Red Fox Jumps Over The Lazy Dog", a48, 8, 330, 1.0f, 1.0f);
        render_text("The Solid Red Fox Jumps Over The Lazy Dog", a48, 28, 450, 1.0f, 1.0f);

        glUniform4fv(uniform_color, 1, transparent_green);
        render_text("The Transparent Green Fox Jumps Over The Lazy Dog", a48, 0, 380, 1.0f, 1.0f);
        render_text("The Transparent Green Fox Jumps Over The Lazy Dog", a48, 18, 440, 1.0f, 1.0f);
    }

    void free_resources() {
        glDeleteProgram(program);
        glDeleteShader(f);
        glDeleteShader(v);
    }
};

struct microatlas {
    GLint width;
    GLint height;
    // Maximum texture width
    #define MAXWIDTH 1024

    /**
 * The atlas struct holds a texture that contains the visible US-ASCII characters
 * of a certain font rendered with a certain character height.
 * It also contains an array that contains all the information necessary to
 * generate the appropriate vertex and texture coordinates for each character.
 *
 * After the constructor is run, you don't need to use any FreeType functions anymore.
 */
    struct atlas {
        GLuint tex = 0;		// texture object

        unsigned int w = 0;			// width of texture in pixels
        unsigned int h = 0;			// height of texture in pixels
        unsigned int size = 0;      // height of character in pixels

        struct {
            float ax = 0.0f;	// advance.x
            float ay = 0.0f;	// advance.y

            float bw = 0.0f;	// bitmap.width;
            float bh = 0.0f;	// bitmap.height;

            float bl = 0.0f;	// bitmap_left;
            float bt = 0.0f;	// bitmap_top;

            float tx = 0.0f;	// x offset of glyph in texture coordinates
            float ty = 0.0f;	// y offset of glyph in texture coordinates
        } c[128];		// character information

        void init(FT_Face face, int height) {
            size = height;
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
            printf("adding rowh (%d) to h (%d)\n", rowh, h);
            h += rowh;
            printf("added rowh (%d) to h (%d)\n", rowh, h);

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

        void denit() {
            LOG_MAGNUM_WARNING_FUNCTION(tex);
            glDeleteTextures(1, &tex);
        }

        atlas(FT_Face face, int height) {
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

        atlas () {
            std::cout << "atlas constructor" << std::endl << std::flush;
        }

        /* Copy constructor */
        constexpr atlas(const atlas &p2) {
            std::cout << "atlas copy constructor" << std::endl << std::flush;
            this->size = p2.size;
            this->tex = p2.tex;
            this->w = p2.w;
            this->h = p2.h;
//            this->font_source = p2.font_source;
            for (int i = 0; i != 128; i++) this->c[i] = p2.c[i];
        }

        constexpr atlas(atlas &&p2) {
            std::cout << "atlas move constructor" << std::endl << std::flush;
            std::swap(this->size, p2.size);
            std::swap(this->tex, p2.tex);
            std::swap(this->w, p2.w);
            std::swap(this->h, p2.h);
//            std::swap(this->font_source, p2.font_source);
            for (int i = 0; i != 128; i++) std::swap(this->c[i], p2.c[i]);
        }

        atlas &operator=(const atlas &p2) {
            std::cout << "atlas copy assignment" << std::endl << std::flush;
            this->size = p2.size;
            this->tex = p2.tex;
            this->w = p2.w;
            this->h = p2.h;
//            this->font_source = p2.font_source;
            for (int i = 0; i != 128; i++) this->c[i] = p2.c[i];
            return *const_cast<atlas*>(this);
        }

        atlas &operator=(atlas &&p2) {
            std::cout << "atlas move assignment" << std::endl << std::flush;
            std::swap(this->size, p2.size);
            std::swap(this->tex, p2.tex);
            std::swap(this->w, p2.w);
            std::swap(this->h, p2.h);
//            std::swap(this->font_source, p2.font_source);
            for (int i = 0; i != 128; i++) std::swap(this->c[i], p2.c[i]);
            return *const_cast<atlas*>(this);
        }

        ~atlas () {
            std::cout << "atlas destructor" << std::endl << std::flush;
            denit();
        }
    };

    struct font_init {
        struct point {
            GLfloat x;
            GLfloat y;
            GLfloat s;
            GLfloat t;
        };

        GLuint program;
        GLuint v;
        GLuint f;
        GLint attribute_coord;
        GLint uniform_tex;
        GLint uniform_color;
        GLuint vbo;
        FT_Library ft;
        FT_Face face;
        bool ft_initialized = false;
        char * font_path = const_cast<char*>("");
        const char * fontfilename;
        bool hasProgram = false;
        atlas * a12;
        atlas * a24;
        atlas * a48;

        int init_resources() {
            /* Initialize the FreeType2 library */
            if (FT_Init_FreeType(&ft)) {
                fprintf(stderr, "Could not init freetype library\n");
                return 0;
            }

            /* Load a font */
            if (FT_New_Face(ft, fontfilename, 0, &face)) {
                fprintf(stderr, "Could not open font %s\n", fontfilename);
                return 0;
            }

            const char *vs = R"glsl( #version 300 es
    layout (location = 0) in vec4 coord;
    out vec2 texpos;
    uniform mat4 projection;

    void main(void) {
        gl_Position = projection * vec4(coord.xy, 0, 1);
        texpos = coord.zw;
    }
    )glsl";

            const char *fs = R"glsl( #version 300 es
    precision mediump float;

    in vec2 texpos;
    uniform sampler2D tex;
    uniform vec4 color;
    out vec4 c;

    void main(void) {
        c = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
    }
    )glsl";

            glis.GLIS_build_simple_shader_program(
                    v, vs,
                    f, fs,
                    program
            );
            if(program == 0)
                return 0;

            attribute_coord = glGetAttribLocation(program, "coord");
            uniform_tex = glGetUniformLocation(program, "tex");
            uniform_color = glGetUniformLocation(program, "color");

            if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
                return 0;

            // Create the vertex buffer object
            glGenBuffers(1, &vbo);

            /* Create texture atlasses for several font sizes */
            a48 = new atlas(face, 48);
            a24 = new atlas(face, 24);
            a12 = new atlas(face, 12);

            return 1;
        }

        font_init() {
            std::cout << "font_init default constructor" << std::endl << std::flush;
        }

        font_init(const char * path) {
            std::cout << "font_init path constructor" << std::endl << std::flush;
            font_path = const_cast<char *>(path);
            if (!ft_initialized) {
                /* Initialize the FreeType2 library */
                if (FT_Init_FreeType(&ft)) {
                    fprintf(stderr, "Could not init freetype library\n");
                    ft_initialized = false;
                    return;
                }
                ft_initialized = true;
            }
        }

        font_init(const font_init &p2) {
            std::cout << "font_init copy constructor" << std::endl << std::flush;
            strcpy(this->font_path, p2.font_path);
            this->ft_initialized = p2.ft_initialized;
            this->ft = p2.ft;
            this->face = p2.face;
            this->hasProgram = p2.hasProgram;
            std::swap(this->v, const_cast<font_init&>(p2).v);
            std::swap(this->f, const_cast<font_init&>(p2).f);
            std::swap(this->program, const_cast<font_init&>(p2).program);
            std::swap(this->uniform_color, const_cast<font_init&>(p2).uniform_color);
            std::swap(this->uniform_tex, const_cast<font_init&>(p2).uniform_tex);
            std::swap(this->attribute_coord, const_cast<font_init&>(p2).attribute_coord);
            std::swap(this->vbo, const_cast<font_init&>(p2).vbo);
        }

        font_init(font_init &&p2) {
            std::cout << "font_init move constructor" << std::endl << std::flush;
            std::swap(this->font_path, p2.font_path);
            std::swap(this->ft_initialized, p2.ft_initialized);
            std::swap(this->ft, p2.ft);
            std::swap(this->face, p2.face);
            std::swap(this->hasProgram, p2.hasProgram);
            std::swap(this->v, p2.v);
            std::swap(this->f, p2.f);
            std::swap(this->program, p2.program);
            std::swap(this->uniform_color, p2.uniform_color);
            std::swap(this->uniform_tex, p2.uniform_tex);
            std::swap(this->attribute_coord, p2.attribute_coord);
            std::swap(this->vbo, p2.vbo);
        }

        font_init &operator=(const font_init &p2) {
            LOG_MAGNUM_WARNING << "font_init copy assignment";
            fflush(stdout);
            strcpy(this->font_path, p2.font_path);
            this->ft_initialized = p2.ft_initialized;
            this->ft = p2.ft;
            this->face = p2.face;
            this->hasProgram = p2.hasProgram;
            std::swap(this->v, const_cast<font_init&>(p2).v);
            std::swap(this->f, const_cast<font_init&>(p2).f);
            std::swap(this->program, const_cast<font_init&>(p2).program);
            std::swap(this->uniform_color, const_cast<font_init&>(p2).uniform_color);
            std::swap(this->uniform_tex, const_cast<font_init&>(p2).uniform_tex);
            std::swap(this->attribute_coord, const_cast<font_init&>(p2).attribute_coord);
            std::swap(this->vbo, const_cast<font_init&>(p2).vbo);
            return *const_cast<font_init*>(this);
        }

        font_init &operator=(font_init &&p2) {
            LOG_MAGNUM_WARNING << "font_init move assignment";
            fflush(stdout);
            std::swap(this->font_path, p2.font_path);
            std::swap(this->ft_initialized, p2.ft_initialized);
            std::swap(this->ft, p2.ft);
            std::swap(this->face, p2.face);
            std::swap(this->hasProgram, p2.hasProgram);
            std::swap(this->v, p2.v);
            std::swap(this->f, p2.f);
            std::swap(this->program, p2.program);
            std::swap(this->uniform_color, p2.uniform_color);
            std::swap(this->uniform_tex, p2.uniform_tex);
            std::swap(this->attribute_coord, p2.attribute_coord);
            std::swap(this->vbo, p2.vbo);
            return *const_cast<font_init*>(this);
        }

        ~font_init () {
            std::cout << "font_init destructor" << std::endl << std::flush;
            if (hasProgram) {
                glDeleteProgram(program);
                glDeleteShader(f);
                glDeleteShader(v);
                hasProgram = false;
            }
        }

        bool load() {
            /* Load a font */
            if (FT_New_Face(ft, font_path, 0, &face)) {
                fprintf(stderr, "Could not open font %s\n", font_path);
                return false;
            }
            const char *vs = R"glsl( #version 300 es
    layout (location = 0) in vec4 coord;
    out vec2 texpos;
    uniform mat4 projection;

    void main(void) {
        gl_Position = projection * vec4(coord.xy, 0, 1);
        texpos = coord.zw;
    }
    )glsl";

            const char *fs = R"glsl( #version 300 es
    precision mediump float;

    in vec2 texpos;
    uniform sampler2D tex;
    uniform vec4 color;
    out vec4 c;

    void main(void) {
        c = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
    }
    )glsl";

            glis.GLIS_build_simple_shader_program(
                    v, vs,
                    f, fs,
                    program
            );
            if(program == 0) return false;

            hasProgram = true;

            attribute_coord = glGetAttribLocation(program, "coord");
            uniform_tex = glGetUniformLocation(program, "tex");
            uniform_color = glGetUniformLocation(program, "color");

            if(attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
                return false;

            // Create the vertex buffer object
            glGenBuffers(1, &vbo);

            a12 = new atlas();
            a12->init(face, 12);
            a24 = new atlas();
            a24->init(face, 24);
            a48 = new atlas();
            a48->init(face, 48);
            return true;
        }
    };

    Kernel font;

    template <typename A, typename B, typename C, typename D> class quad {
    public:
        A first;
        B second;
        C third;
        D fourth;
        quad(A first, B second, C third, D fourth) {
            this->first = first;
            this->second = second;
            this->third = third;
            this->fourth = fourth;
        }
    };

    struct font_data {
        Kernel sizes;

        font_data() {
            std::cout << "font_data default constructor" << std::endl << std::flush;
        }

        font_data(const Kernel & kernel) {
            std::cout << "font_data kernel constructor" << std::endl << std::flush;
            sizes = kernel;
        }

        /* Copy constructor */
        font_data(const font_data &p2) {
            std::cout << "font_data copy constructor" << std::endl << std::flush;
            this->sizes = p2.sizes;
        }

        font_data(font_data &&p2) {
            std::cout << "font_data move constructor" << std::endl << std::flush;
            std::swap(this->sizes, p2.sizes);
        }

        font_data &operator=(const font_data &p2) {
            LOG_MAGNUM_WARNING << "font_data copy assignment";
            this->sizes = p2.sizes;
            return *const_cast<font_data*>(this);
        }

        font_data &operator=(font_data &&p2) {
            LOG_MAGNUM_WARNING << "font_data move assignment";
            std::swap(this->sizes, p2.sizes);
            return *const_cast<font_data*>(this);
        }

        ~font_data () {
            std::cout << "font_data destructor" << std::endl << std::flush;
        }
    };

    typedef quad <const char*, const char*, AnyOpt, AnyOpt> ATLAS_TYPE;

    bool add_font(const char * id, const char * path) {
        font_init * i = new font_init(path);
        i->load();
        return font.newObject(
                0,
                0,
                ATLAS_TYPE {
                    id,
                    path,
                    AnyOpt(i, true),
                    AnyOpt(new font_data(Kernel()), true)
                }
        ) != nullptr;
    }

    Object * find_font(const char * id) {
        size_t resource_len = strlen(id);
        int page = 1;
        size_t index = 0;
        size_t page_size = font.table->page_size;
        printf("font.table->Page.count() is %d\n", font.table->Page.count());
        for (; page <= font.table->Page.count(); page++) {
            index = ((page_size * page) - page_size);
            for (; index < page_size * page; index++)
                if (font.table->table[index] != nullptr) {
                    const char * data = font.table->table[index]->resource.get<ATLAS_TYPE*>()->first;
                    if (!memcmp(data, id, resource_len))
                        return font.table->table[index];
                } else {
                    printf("font.table->table[%zu] is nullptr\n", index);
                }
        }
        return nullptr;
    }

    ATLAS_TYPE * get_atlas(const char * id) {
        auto o = find_font(id);
        if (o == nullptr) return nullptr;
        return o->resource.get<ATLAS_TYPE*>();
    }

    bool add_font_size(const char * id, int size) {
        ATLAS_TYPE * data = get_atlas(id);
        if (data == nullptr) return false;
        auto fontInit = data->third.get<font_init*>();
        auto fontData = data->fourth.get<font_data*>();
        atlas x = atlas();
        x.init(fontInit->face, size);
//        x.font_source = fontInit;
//        LOG_MAGNUM_WARNING_FUNCTION(x.size);
//        LOG_MAGNUM_WARNING << "adding new object";
        Object * o = fontData->sizes.newObject(0, 0, x);
//        LOG_MAGNUM_WARNING << "added new object";
//        LOG_MAGNUM_WARNING << "asserting object contains size";
//        assert(o->resource.has_value());
//        assert(o->resource.get<atlas>().size == size);
//        assert(o->resource.get<atlas*>()->size == size);
//        LOG_MAGNUM_WARNING << "object contains size";
//        LOG_MAGNUM_WARNING << "asserting object that contains size can be found";
//        assert(find_size("id", fontData, size) != nullptr);
//        LOG_MAGNUM_WARNING << "object that contains size was found";
        return o != nullptr;
    }

    atlas * find_size(const char * id, font_data * fontData, int size) {
        Kernel * x = &fontData->sizes;
        if (x->table->Page.count() == 0) {
            printf("font %s has no sizes created\n", id);
        } else {
            int page = 1;
            size_t index = 0;
            size_t page_size = x->table->page_size;
            for (; page <= x->table->Page.count(); page++) {
                index = ((page_size * page) - page_size);
                for (; index < page_size * page; index++)
                    if (x->table->table[index] != nullptr) {
                        if (x->table->table[index]->resource.has_value()) {
                            printf("checking index %zu\n", index);
                            atlas * atlas_ = x->table->table[index]->resource.get<atlas*>();
                            if (atlas_->size == size) {
                                printf("found requested size at index %zu\n", index);
                                return atlas_;
                            }
                        } else
                            printf("index %zu has no resource\n", index);
                    }
            }
        }
        printf("failed to find size %d\n", size);
        return nullptr;
    }

    atlas * find_size(const char * id, int size) {
        ATLAS_TYPE * data = get_atlas(id);
        if (data == nullptr) return nullptr;
        font_data * fontData = data->fourth.get<font_data*>();
        assert(fontData != nullptr);
        return find_size(id, fontData, size);
    }

    const char * fontfilename;
    font_init fontInit;

    void init_resources() {
        fontInit.fontfilename = fontfilename;
        fontInit.init_resources();
    }

    /**
     * Render text using the currently loaded font and currently set font size.
     * Rendering starts at coordinates (x, y), z is always 0.
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, font_init * fontInit, atlas * a, float x, float y, float sx, float sy) {
        // flip y axis
        float y_ = static_cast<float>(height)-y;

        printf("text: %s\n", text);
        printf("x: %G, y: %G\n", x, y);
        printf("sx: %G, sy: %G\n", 1.0f, 1.0f);
        const uint8_t *p;

        /* Use the texture containing the atlas */
        glBindTexture(GL_TEXTURE_2D, a->tex);
        glUniform1i(fontInit->uniform_tex, 0);
        glUniformMatrix4fv(glGetUniformLocation(fontInit->program, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height))));

        /* Set up the VBO for our vertex data */
        glEnableVertexAttribArray(fontInit->attribute_coord);
        glBindBuffer(GL_ARRAY_BUFFER, fontInit->vbo);
        glVertexAttribPointer(fontInit->attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

        typedef font_init::point point;

        point coords[6 * strlen(text)];
        int c = 0;

        /* Loop through all characters */
        for (p = (const uint8_t *)text; *p; p++) {
            /* Calculate the vertex and texture coordinates */
            float x2 = x + a->c[*p].bl * sx;
            float y2 = -y_ - a->c[*p].bt * sy;
            float w = a->c[*p].bw * sx;
            float h = a->c[*p].bh * sy;

            /* Advance the cursor to the start of the next character */
            x += a->c[*p].ax * sx;
            y += a->c[*p].ay * sy;

            /* Skip glyphs that have no pixels */
            if ((w == 0.0f) || (h == 0.0f))
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

        glDisableVertexAttribArray(fontInit->attribute_coord);
    }

    /**
     * Render text using the currently loaded font and currently set font size.
     * Rendering starts at coordinates (x, y), z is always 0.
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char * font_id, const int font_size, float x, float y, float sx, float sy) {
        ATLAS_TYPE * data = get_atlas(font_id);
        if (data == nullptr) return;
        atlas * a = find_size(font_id, data->fourth.get<font_data*>(), font_size);
        render_text(text, data->third.get<font_init*>(), a, x, y, sx, sy);
    }

    void display(font_init * fontInit, atlas * a12, atlas* a24, atlas * a48) {
        glUseProgram(fontInit->program);

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
        glUniform4fv(fontInit->uniform_color, 1, black);

        /* Effects of alignment */
        render_text("The Quick Brown Fox Jumps Over The Lazy Dog", fontInit, a48, 8, 50, 1.0f, 1.0f);
        render_text("The Misaligned Fox Jumps Over The Lazy Dog", fontInit, a48, 8.5, 100.5, 1.0f, 1.0f);

        /* Scaling the texture versus changing the font size */
        render_text("The Small Texture Scaled Fox Jumps Over The Lazy Dog", fontInit, a48, 8, 175, 0.5f, 0.5f);
        render_text("The Small Font Sized Fox Jumps Over The Lazy Dog", fontInit, a24, 8, 200, 1.0f, 1.0f);
        render_text("The Tiny Texture Scaled Fox Jumps Over The Lazy Dog", fontInit, a48, 8, 235, 0.25, 0.25);
        render_text("The Tiny Font Sized Fox Jumps Over The Lazy Dog", fontInit, a12, 8, 250, 1.0f, 1.0f);

        /* Colors and transparency */
        render_text("The Solid Black Fox Jumps Over The Lazy Dog", fontInit, a48, 8, 430, 1.0f, 1.0f);

        glUniform4fv(fontInit->uniform_color, 1, red);
        render_text("The Solid Red Fox Jumps Over The Lazy Dog", fontInit, a48, 8, 330, 1.0f, 1.0f);
        render_text("The Solid Red Fox Jumps Over The Lazy Dog", fontInit, a48, 28, 450, 1.0f, 1.0f);

        glUniform4fv(fontInit->uniform_color, 1, transparent_green);
        render_text("The Transparent Green Fox Jumps Over The Lazy Dog", fontInit, a48, 0, 380, 1.0f, 1.0f);
        render_text("The Transparent Green Fox Jumps Over The Lazy Dog", fontInit, a48, 18, 440, 1.0f, 1.0f);
    }

    void display() {
        display(&fontInit, fontInit.a12, fontInit.a24, fontInit.a48);
    }
};

GLuint texture;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

miniatlas a;
microatlas b;

microatlas::atlas * a12;
microatlas::atlas * a24;
microatlas::atlas * a48;
microatlas::font_init * fontInit;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
//    a.display();
    b.display(fontInit, a12, a24, a48);
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glViewport(0, 0, renderer.width, renderer.height);
    font.GLIS_font_set_RenderText_w_h(renderer.width, renderer.height);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
//    a.free_resources();
//    b.free_resources();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 640, 480);
    glis.GLIS_setupOnScreenRendering(screen);
    glViewport(0, 0, screen.width, screen.height);
    auto f = "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf";
//    a.fontfilename = f;
//    a.width = screen.width;
//    a.height = screen.height;
//    a.init_resources();
//    a.display();
    b.fontfilename = f;
    b.width = screen.width;
    b.height = screen.height;
    b.init_resources();
    b.display();

//    b.width = screen.width;
//    b.height = screen.height;
//    LOG_MAGNUM_WARNING << "adding font";
//    assert(b.add_font("id", f));
//    LOG_MAGNUM_WARNING << "adding sizes";
//    assert(b.add_font_size("id", 12));
//    assert(b.add_font_size("id", 24));
//    assert(b.add_font_size("id", 48));
//    microatlas::ATLAS_TYPE * data = b.get_atlas("id");
//    if (data == nullptr) return -1;
//    fontInit = data->third.get<microatlas::font_init*>();
//    auto fontData = data->fourth.get<microatlas::font_data*>();
//    a12 = b.find_size("id", fontData, 12);
//    a24 = b.find_size("id", fontData, 24);
//    a48 = b.find_size("id", fontData, 48);
//    microatlas::font_init * fontInit = data->third.get<microatlas::font_init*>();
//    b.width = screen.width;
//    b.height = screen.height;
//    microatlas::font_init * fontInit = new microatlas::font_init();
//    fontInit->fontfilename = f;
//    fontInit->init_resources();
//    microatlas::atlas A = microatlas::atlas();
//    microatlas::atlas B = microatlas::atlas();
//    microatlas::atlas C = microatlas::atlas();
//    A.init(fontInit->face, 12);
//    B.init(fontInit->face, 24);
//    C.init(fontInit->face, 48);
//    b.width = screen.width;
//    b.height = screen.height;
//    b.init_resources();
//    b.display();
//    microatlas::font_init * fontInit = new microatlas::font_init();
//    fontInit->fontfilename = f;
//    fontInit->init_resources();
//    b.display(b.fontInit, b.fontInit->a12, b.fontInit->a24, b.fontInit->a48);
    glis.GLIS_SwapBuffers(screen);
    glis.runUntilX11WindowClose(glis, screen, font, fps, nullptr, resize, close);
}