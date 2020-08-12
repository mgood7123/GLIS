#pragma once

#include <glis/internal/glis_class.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <freetype/ftbbox.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Magnum/GL/OpenGL.h>

class GLIS_FONT {
public:
    GLint width;
    GLint height;
    void set_max_width_height(GLint width, GLint height);

    void set_max_width_height(GLIS_CLASS & screen);

    // Maximum texture width
    static const int MAXWIDTH  = 1024;

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
        char *font_path = nullptr;
        bool hasProgram = false;

        font_init();

        font_init(const char *path);

        font_init(const font_init &p2);

        font_init(font_init &&p2);

        font_init &operator=(const font_init &p2);

        font_init &operator=(font_init &&p2);

        ~font_init();

        bool load();
    };

    /**
     * The atlas struct holds a texture that contains the visible US-ASCII characters
     * of a certain font rendered with a certain character height.
     * It also contains an array that contains all the information necessary to
     * generate the appropriate vertex and texture coordinates for each character.
     *
     * After the constructor is run, you don't need to use any FreeType functions anymore.
     */
    struct atlas {
        GLuint tex = 0;        // texture object

        unsigned int w = 0;            // width of texture in pixels
        unsigned int h = 0;            // height of texture in pixels
        unsigned int size = 0;      // height of character in pixels

        // pointer to the loaded font in which this atlas is constructed
        font_init *font_source = nullptr;

        struct {
            float ax = 0.0f;    // advance.x
            float ay = 0.0f;    // advance.y

            float bw = 0.0f;    // bitmap.width;
            float bh = 0.0f;    // bitmap.height;

            float bl = 0.0f;    // bitmap_left;
            float bt = 0.0f;    // bitmap_top;

            float tx = 0.0f;    // x offset of glyph in texture coordinates
            float ty = 0.0f;    // y offset of glyph in texture coordinates
        } c[128];        // character information

        void init(FT_Face face, int height);

        void denit();

        atlas(FT_Face face, int height);

        atlas();

        constexpr atlas(const atlas &p2);

        constexpr atlas(atlas &&p2);

        atlas &operator=(const atlas &p2);

        atlas &operator=(atlas &&p2);

        ~atlas();
    };

    Kernel font;

    template<typename A, typename B, typename C, typename D>
    class quad {
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

        font_data();

        font_data(const Kernel &kernel);

        /* Copy constructor */
        font_data(const font_data &p2);

        font_data(font_data &&p2);

        font_data &operator=(const font_data &p2);

        font_data &operator=(font_data &&p2);

        ~font_data();
    };

    typedef quad<const char *, const char *, AnyOpt, AnyOpt> ATLAS_TYPE;

    ATLAS_TYPE * add_font(const char *id, const char *path);

    Object *find_font(const char *id);

    ATLAS_TYPE *get_atlas(const char *id);

    atlas * add_font_size(const char *id, int size);

    atlas *find_size(const char *id, font_data *fontData, int size);

    atlas *find_size(const char *id, int size);

    struct colors {
        const GLfloat black[4] = { 0, 0, 0, 1 };
        const GLfloat white[4] = { 1, 1, 1, 1 };
        const GLfloat red[4] = { 1, 0, 0, 1 };
        const GLfloat transparent_green[4] = { 0, 1, 0, 0.5 };
    };

    colors colors;

    GLfloat * current_color = const_cast<GLfloat *>(colors.black);

    void set_color(const GLfloat color[4]);

    /**
     * Render text using the specified font atlas, with the specified color,
     * with the specified x and y scaling.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the specified color is nullptr, 0x0, NULL, or equivalent values
     * then the currently set color is used.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas *a, float x, float y, float sx, float sy,
                     const GLfloat * color);

    /**
     * Render text using the specified font atlas, with the currently set color,
     * with the specified x and y scaling.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas *a, float x, float y, float sx, float sy);

    /**
     * Render text using the specified font and size, with the specified color,
     * with the specified x and y scaling.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the specified color is nullptr, 0x0, NULL, or equivalent values
     * then the currently set color is used.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                     float sx, float sy, const GLfloat * color);

    /**
     * Render text using the specified font and size, with the currently set color,
     * with the specified x and y scaling.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                     float sx, float sy);

    /**
     * Render text using the specified font atlas, with the specified color,
     * with the x and y scaling set to 1.0f.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the specified color is nullptr, 0x0, NULL, or equivalent values
     * then the currently set color is used.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas *a, float x, float y, const GLfloat * color);

    /**
     * Render text using the specified font atlas, with the currently set color,
     * with the x and y scaling set to 1.0f.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas *a, float x, float y);

    /**
     * Render text using the specified font and size, with the specified color,
     * with the x and y scaling set to 1.0f.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the specified color is nullptr, 0x0, NULL, or equivalent values
     * then the currently set color is used.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                     const GLfloat * color);

    /**
     * Render text using the specified font and size, with the currently set color,
     * with the x and y scaling set to 1.0f.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char *font_id, const int font_size, float x, float y);

    /**
     * Render text using the specified font atlas, with the specified color,
     * with the specified scaling for both x and y.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the specified color is nullptr, 0x0, NULL, or equivalent values
     * then the currently set color is used.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas *a, float x, float y, float sxy,
            const GLfloat * color);

    /**
     * Render text using the specified font atlas, with the currently set color,
     * with the specified scaling for both x and y.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, atlas *a, float x, float y, float sxy);

    /**
     * Render text using the specified font and size, with the specified color,
     * with the specified scaling for both x and y.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the specified color is nullptr, 0x0, NULL, or equivalent values
     * then the currently set color is used.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                     float sxy, const GLfloat * color);

    /**
     * Render text using the specified font and size, with the currently set color,
     * with the specified scaling for both x and y.
     *
     * if the scaling is 1.0f, no scaling is applied.
     *
     * if the scaling is larger than 1.0f, for example 1.1f, 2.5f, and so on
     * then the text will appear larger.
     *
     * if the scaling is smaller than 1.0f, for example 0.9f, 0.5f, and so on
     * then the text will appear smaller.
     *
     * If the currently set color is nullptr, 0x0, NULL, or equivalent values
     * then the color black is used
     *
     * Rendering starts at coordinates (x, y), z is always 0.
     * With (x = 0, y = 0) being top left.
     *
     * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
     */
    void render_text(const char *text, const char *font_id, const int font_size, float x, float y,
            float sxy);
};
