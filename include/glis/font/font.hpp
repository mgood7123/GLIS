#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <freetype/ftbbox.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Magnum/GL/OpenGL.h>

extern GLint attribute_coord;
extern GLint uniform_projection;
extern GLint uniform_tex;
extern GLint uniform_color;

class GLIS_FONT {
public:

    // TODO: remove

    FT_Library GLIS_font;

    int width = 0;
    int height = 0;

    int max_width = 0;
    int max_height = 0;

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

    struct point {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    };

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
    } texture_atlas;

    std::map<GLchar, Character> sCharacters;

    bool GLIS_font_2D_store_ascii();
    bool GLIS_font_2D_store_ascii_atlas();

    void GLIS_font_free();

    GLuint GLIS_FONT_SHADER_PROGRAM;
    GLuint GLIS_FONT_VERTEX_SHADER;
    GLuint GLIS_FONT_FRAGMENT_SHADER;
    GLuint GLIS_FONT_VAO;
    GLuint GLIS_FONT_VBO;

    bool GLIS_font_init_shaders();

    bool GLIS_load_font(const char *font, int width, int height);

    void GLIS_font_RenderText(int w, int h, std::string text, int x, int y,
                              unsigned int scale, glm::vec3 color);

    void GLIS_font_set_RenderText_w_h(int w, int h);

    void GLIS_font_RenderText(std::string text, int x, int y, glm::vec3 color);

    glm::vec3 GLIS_font_color_black = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 GLIS_font_color_white = glm::vec3(1.0f, 1.0f, 1.0f);

    void GLIS_font_RenderTextDNC(std::string text, float DNC_x, float DNC_y, glm::vec3 color);
    void GLIS_font_RenderTextDNC(int w, int h, std::string text, float DNC_x, float DNC_y, float scale,
                              glm::vec3 color);







    struct ATLAS {
        GLuint program;
        GLuint v;
        GLuint f;
        GLint attribute_coord;
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
        static const int MAXWIDTH = 1024;

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

            atlas(FT_Face face, int height);

            ~atlas();
        };

        // a map of atlas's corresponding to loaded fonts and sizes

        // an atlas map tailored specifically for font access

//        atlas_map<const char *, atlas_map<const char *, atlas*>> atlas_index;

//        typedef std::map<int, atlas*> atlas_map;
//        std::map<const char *,  atlas_map *> atlas_index;

        atlas *a48;
        atlas *a24;
        atlas *a12;

        ATLAS();

        static GLint attribute_coord_;
        static GLint uniform_projection_;
        static GLint uniform_tex_;
        static GLint uniform_color_;

        int width;
        int height;

        void set_width_height(int w, int h);

        int load_font(const char * font_name, const char * font_path);

        void generate_font_size(int size);

        /**
         * Render text using the currently loaded font and currently set font size.
         * Rendering starts at coordinates (x, y), z is always 0.
         * The pixel coordinates that the FreeType2 library uses are scaled by (1.0f, 1.0f).
         */
        void render_text(const char *text, GLIS_FONT & font, atlas * a, float x, float y, float sx, float sy);

        void display(GLIS_FONT & font);

        void free_resources();
    } A;
};