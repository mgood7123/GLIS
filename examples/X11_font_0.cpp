//
// Created by smallville7123 on 14/07/20.
//

// file created in commit c04b9a80992c063cd379ab1d1ce572dcf03bf6cd - "upload"

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

//     in vec2 TexCoords;
//     out vec4 color;
//
//     uniform sampler2D text;
//     uniform vec3 textColor;
//
//     void main()
//     {
//         vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
//         color = vec4(textColor, 1.0) * sampled;
//     }
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

GLuint texture;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

miniatlas a;

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(draw, glis, renderer, font, fps) {
    a.display();
    glis.GLIS_SwapBuffers(screen);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(resize, glis, renderer, font, fps) {
    glViewport(0, 0, renderer.width, renderer.height);
    font.GLIS_font_set_RenderText_w_h(renderer.width, renderer.height);
}

GLIS_CALLBACKS_DRAW_RESIZE_CLOSE(close, glis, renderer, font, fps) {
    glis.destroyX11Window(screen);
    a.free_resources();
    glis.GLIS_destroy_GLIS(screen);
}

int main() {
    glis.getX11Window(screen, 640, 480);
    glis.GLIS_setupOnScreenRendering(screen);
    glViewport(0, 0, screen.width, screen.height);
    a.fontfilename = "/home/smallville7123/AndroidCompositor/app/src/main/jni/executables/fonts/Vera.ttf";
    a.width = screen.width;
    a.height = screen.height;
    a.init_resources();
    glis.runUntilX11WindowClose(glis, screen, font, fps, draw, resize, close);
}