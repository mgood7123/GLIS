// g++ -I /usr/include/freetype2 textrender.cpp -lglfw -lGLEW -lGL -lfreetype && ./a.out

// Std. Includes
#include <iostream>
#include <map>
#include <string>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Properties

bool useAtlas = true;

const GLuint WIDTH = 800, HEIGHT = 600;

GLuint GLIS_FONT_SHADER_PROGRAM;
GLuint GLIS_FONT_VERTEX_SHADER;
GLuint GLIS_FONT_FRAGMENT_SHADER;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

    struct point {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    };

    GLint attribute_coord;
    GLint uniform_projection;
    GLint uniform_tex;
    GLint uniform_color;

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

std::map<GLchar, Character> Characters;
GLuint VAO, VBO;

void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

void GLIS_build_simple_shader_program(
    GLuint & vertexShader, const char *vertexSource,
    GLuint & fragmentShader, const char *fragmentSource,
    GLuint &shaderProgram
);

int LOG_INFO(const char* format, ... );
int LOG_ERROR(const char* format, ... );
void LOG_ALWAYS_FATAL(const char* format, ... );

// The MAIN function, from here we start our application and run the Game loop
int main()
{
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compile and setup the shader
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
    GLIS_build_simple_shader_program(
            GLIS_FONT_VERTEX_SHADER, vs,
            GLIS_FONT_FRAGMENT_SHADER, fs,
            GLIS_FONT_SHADER_PROGRAM
    );
    glUseProgram(GLIS_FONT_SHADER_PROGRAM);
    uniform_projection = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "projection");
    uniform_color = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "color");
    uniform_tex = glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "tex");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WIDTH), 0.0f, static_cast<GLfloat>(HEIGHT));
    glUniformMatrix4fv(glGetUniformLocation(GLIS_FONT_SHADER_PROGRAM, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        LOG_ALWAYS_FATAL("ERROR::FREETYPE: Could not init FreeType Library");

    // Load font as face
    FT_Face face;
    FT_Face & GLIS_font_face = face;
    const char * font = "/usr/share/fonts/truetype/open-sans/OpenSans-Regular.ttf";
    if (FT_New_Face(ft, font, 0, &face))
        LOG_ALWAYS_FATAL("ERROR::FREETYPE: Failed to load font %s", font);

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    if (useAtlas) {
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
    } else {
        // Load first 128 characters of ASCII set
        for (GLubyte c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<GLchar, Character>(c, character));
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    
    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        RenderText("This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        RenderText("(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
       
        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glDeleteProgram(GLIS_FONT_SHADER_PROGRAM);
    glDeleteShader(GLIS_FONT_FRAGMENT_SHADER);
    glDeleteShader(GLIS_FONT_VERTEX_SHADER);
    glfwTerminate();
    return 0;
}

void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    glUseProgram(GLIS_FONT_SHADER_PROGRAM);
    glUniform3f(uniform_color, color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

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
        // Iterate through all characters
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) 
        {
            Character ch = Characters[*c];

            GLfloat xpos = x + ch.Bearing.x * scale;
            // Y bottom
            GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            // y top
//             GLfloat ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

            GLfloat w = ch.Size.x * scale;
            GLfloat h = ch.Size.y * scale;
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                // y bottom
                { xpos,     ypos + h,   0.0, 0.0 },            
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }           
                // y top
//                 { xpos,     ypos + h,   0.0f, 1.0f },
//                 { xpos + w, ypos,       1.0f, 0.0f },
//                 { xpos,     ypos,       0.0f, 0.0f },
// 
//                 { xpos,     ypos + h,   0.0f, 1.0f },
//                 { xpos + w, ypos + h,   1.0f, 1.0f },
//                 { xpos + w, ypos,       1.0f, 0.0f }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}






// GLIS

#include <stdarg.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <mutex>

std::mutex lock;

int LOG_INFO(const char* format, ... ) {
    assert(format != nullptr);
    lock.lock();
#ifdef __ANDROID__
    va_list args2;
    va_start(args2, format);
    __android_log_vprint(ANDROID_LOG_INFO, "GLIS", format, args2);
    va_end(args2);
#endif
    // set color to green
    fprintf(stdout, "\033[0;32m");
    va_list args;
    va_start(args, format);
    int len = vfprintf(stdout, format, args);
    va_end(args);
    // clear color
    fprintf(stdout, "\033[0m");
    len += fprintf(stdout, "\n");
    fflush(stdout);
    lock.unlock();
    return len;
}

int LOG_ERROR(const char* format, ... ) {
    assert(format != nullptr);
    lock.lock();
#ifdef __ANDROID__
    va_list args2;
    va_start(args2, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "GLIS", format, args2);
    va_end(args2);
#endif
    // set color to red
    fprintf(stderr, "\033[0;31m");
    va_list args;
    va_start(args, format);
    int len = vfprintf(stderr, format, args);
    va_end(args);
    // clear color
    fprintf(stderr, "\033[0m");
    len += fprintf(stderr, "\n");
    fflush(stderr);
    lock.unlock();
    return len;
}

void LOG_ALWAYS_FATAL(const char* format, ... ) {
    assert(format != nullptr);
    lock.lock();
#ifdef __ANDROID__
    va_list args2;
    va_start(args2, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "GLIS", format, args2);
    va_end(args2);
#endif
    // set color to red
    fprintf(stderr, "\033[0;31m");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    // clear color
    fprintf(stderr, "\033[0m");
    fprintf(stderr, "\n");
    fflush(stderr);
    lock.unlock();
    abort();
}

std::string GLIS_INTERNAL_MESSAGE_PREFIX = "";

bool GLIS_LOG_PRINT_NON_ERRORS = false;

#define GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGING_FUNCTION, CASE_NAME, name, const, constSTRING, UNNAMED_STRING_CAN_PRINT_ERROR, UNNAMED_STRING_CANNOT_PRINT_ERROR, NAMED_STRING_CAN_PRINT_ERROR, NAMED_STRING_CANNOT_PRINT_ERROR, PRINT, IS_AN_ERROR) CASE_NAME: { \
    if(name == nullptr || name == nullptr || name == 0) { \
        if (PRINT) { \
            if ((UNNAMED_STRING_CAN_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += UNNAMED_STRING_CAN_PRINT_ERROR; \
                LOGGING_FUNCTION(msg.c_str(), constSTRING); \
            } \
        } \
        else { \
            if ((UNNAMED_STRING_CANNOT_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += UNNAMED_STRING_CANNOT_PRINT_ERROR; \
                LOGGING_FUNCTION("%s", msg.c_str()); \
            } \
        } \
    } \
    else { \
        if (PRINT) { \
            if ((NAMED_STRING_CAN_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += NAMED_STRING_CAN_PRINT_ERROR; \
                LOGGING_FUNCTION(msg.c_str(), name, constSTRING); \
            } \
        } \
        else { \
            if ((NAMED_STRING_CANNOT_PRINT_ERROR) != nullptr) { \
                std::string msg = GLIS_INTERNAL_MESSAGE_PREFIX; \
                msg += NAMED_STRING_CANNOT_PRINT_ERROR; \
                LOGGING_FUNCTION(msg.c_str(), name); \
            } \
        } \
    } \
    if (IS_AN_ERROR) abort(); \
    break; \
}


#define GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOGGER, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGER, case const, name, const, constSTRING, nullptr, UNNAMED_STRING, nullptr, NAMED_STRING, false, false)

#define GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING(LOGGER, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING, IS_AN_ERROR) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOGGER, case const, name, const, constSTRING, UNNAMED_STRING, nullptr, NAMED_STRING, nullptr, true, IS_AN_ERROR)

#define GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING(name, const, constSTRING, UNNAMED_STRING, NAMED_STRING) \
    GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING(LOG_ERROR, name, const, constSTRING, UNNAMED_STRING, NAMED_STRING, true)

#define GLIS_ERROR_SWITCH_CASE(name, const) \
    GLIS_ERROR_SWITCH_CASE_CUSTOM_STRING(name, const, #const, "%s", "%s generated error: %s")

#define GLIS_ERROR_SWITCH_CASE_DEFAULT(name, err) \
    GLIS_SWITCH_CASE_CUSTOM_CASE_CUSTOM_LOGGER_CUSTOM_STRING_CAN_I_PRINT_ERROR(LOG_ERROR, default, name, err, err, "Unknown error: %d", "Unknown error", "%s generated an unknown error: %d", "%s generated an unknown error", true, true)

#define GLIS_boolean_to_string(val, TRUE_VALUE) val == TRUE_VALUE ? "true" : "false"

void GLIS_error_to_string_GL(const char *name, GLint err) {
    GLIS_INTERNAL_MESSAGE_PREFIX = "OpenGL:          ";
    switch (err) {
        // GENERATED BY glGetError() ITSELF
        GLIS_SWITCH_CASE_CUSTOM_LOGGER_CUSTOM_STRING_DONT_PRINT_ERROR(LOG_INFO, name,
                                                                      GL_NO_ERROR,
                                                                      "GL_NO_ERROR",
                                                                      GLIS_LOG_PRINT_NON_ERRORS
                                                                      ? "no error was generated"
                                                                      : nullptr,
                                                                      GLIS_LOG_PRINT_NON_ERRORS
                                                                      ? "%s did not generate an error"
                                                                      : nullptr)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_ENUM)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_VALUE)
        GLIS_ERROR_SWITCH_CASE(name, GL_INVALID_OPERATION)
        GLIS_ERROR_SWITCH_CASE(name, GL_OUT_OF_MEMORY)

        // WHEN ALL ELSE FAILS
        GLIS_ERROR_SWITCH_CASE_DEFAULT(name, err)
    }
    GLIS_INTERNAL_MESSAGE_PREFIX = "";
}

void GLIS_error_to_string_GL(const char *name) {
    GLIS_error_to_string_GL(name, glGetError());
}

// glis shader compiler

GLboolean GLIS_ShaderCompilerSupported() {
    GLboolean GLSC_supported;
    glGetBooleanv(GL_SHADER_COMPILER, &GLSC_supported);
    GLIS_error_to_string_GL("glGetBooleanv(GL_SHADER_COMPILER, &GLSC_supported)");
    LOG_INFO("Supports Shader Compiler: %s", GLIS_boolean_to_string(GLSC_supported, GL_TRUE));
    return GLSC_supported;
}

GLuint GLIS_createShader(GLenum shaderType, const char *&src) {
    if (GLIS_ShaderCompilerSupported()) {
        const char *SHADER_TYPE = nullptr;
        switch (shaderType) {
            case GL_COMPUTE_SHADER:
                SHADER_TYPE = "Compute";
                break;
            case GL_FRAGMENT_SHADER:
                SHADER_TYPE = "Fragment";
                break;
            case GL_GEOMETRY_SHADER:
                SHADER_TYPE = "Geometry";
                break;
            case GL_VERTEX_SHADER:
                SHADER_TYPE = "Vertex";
                break;
            default:
                SHADER_TYPE = "Unknown";
                break;
        }
        LOG_INFO("Creating %s Shader", SHADER_TYPE);
        GLuint shader = glCreateShader(shaderType);
        GLIS_error_to_string_GL("glCreateShader");
        if (!shader) {
            return 0;
        }
        glShaderSource(shader, 1, &src, nullptr);
        GLIS_error_to_string_GL("glShaderSource");
        LOG_INFO("Created %s Shader", SHADER_TYPE);
        GLint compiled = GL_FALSE;
        LOG_INFO("Compiling %s Shader", SHADER_TYPE);
        glCompileShader(shader);
        GLIS_error_to_string_GL("glCompileShader");
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        GLIS_error_to_string_GL("glGetShaderiv");
        if (compiled != GL_TRUE) {
            GLint infoLogLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
            GLIS_error_to_string_GL("glGetShaderiv");
            if (infoLogLen > 0) {
                GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
                if (infoLog) {

                    glGetShaderInfoLog(shader, infoLogLen, nullptr, infoLog);
                    GLIS_error_to_string_GL("glGetShaderInfoLog");
                    LOG_ERROR("Could not compile %s shader:\n%s", SHADER_TYPE, infoLog);
                    free(infoLog);
                }
            }
            glDeleteShader(shader);
            GLIS_error_to_string_GL("glDeleteShader");
            return 0;
        }
        assert(glIsShader(shader) == GL_TRUE);
        LOG_INFO("Compiled %s Shader", SHADER_TYPE);
        return shader;
    } else return 0;
}

GLboolean GLIS_validate_program_link(GLuint &Program) {
    GLint linked = GL_FALSE;
    glGetProgramiv(Program, GL_LINK_STATUS, &linked);
    GLIS_error_to_string_GL("glGetProgramiv");
    if (linked != GL_TRUE) {
        GLint infoLogLen = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLen);
        GLIS_error_to_string_GL("glGetProgramiv");
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {

                glGetProgramInfoLog(Program, infoLogLen, nullptr, infoLog);
                GLIS_error_to_string_GL("glGetProgramInfoLog");
                LOG_ERROR("Could not link program:\n%s", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(Program);
        GLIS_error_to_string_GL("glDeleteProgram");
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean GLIS_validate_program_valid(GLuint &Program) {
    GLint validated = GL_FALSE;
    glValidateProgram(Program);
    GLIS_error_to_string_GL("glValidateProgram");
    glGetProgramiv(Program, GL_VALIDATE_STATUS, &validated);
    GLIS_error_to_string_GL("glGetProgramiv");
    if (validated != GL_TRUE) {
        GLint infoLogLen = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &infoLogLen);
        GLIS_error_to_string_GL("glGetProgramiv");
        if (infoLogLen > 0) {
            GLchar *infoLog = (GLchar *) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {

                glGetProgramInfoLog(Program, infoLogLen, nullptr, infoLog);
                GLIS_error_to_string_GL("glGetProgramInfoLog");
                LOG_ERROR("Could not validate program:\n%s", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(Program);
        GLIS_error_to_string_GL("glDeleteProgram");
        return GL_FALSE;
    }
    return GL_TRUE;
}

GLboolean GLIS_validate_program(GLuint &Program) {
    if (GLIS_validate_program_link(Program) == GL_TRUE)
        if (GLIS_validate_program_valid(Program) == GL_TRUE) {
            GLboolean v = glIsProgram(Program);
            GLIS_error_to_string_GL("glIsProgram");
            return v;
        }
    return GL_FALSE;
}

void GLIS_build_simple_shader_program(
    GLuint & vertexShader, const char *vertexSource,
    GLuint & fragmentShader, const char *fragmentSource,
    GLuint &shaderProgram
) {
    vertexShader = GLIS_createShader(GL_VERTEX_SHADER, vertexSource);
    fragmentShader = GLIS_createShader(GL_FRAGMENT_SHADER, fragmentSource);
    LOG_INFO("Creating Shader program");
    shaderProgram = glCreateProgram();
    GLIS_error_to_string_GL("glCreateProgram");
    LOG_INFO("Created Shader program");
    LOG_INFO("Attaching vertex Shader to program");
    glAttachShader(shaderProgram, vertexShader);
    GLIS_error_to_string_GL("glAttachShader");
    LOG_INFO("Attached vertex Shader to program");
    LOG_INFO("Attaching fragment Shader to program");
    glAttachShader(shaderProgram, fragmentShader);
    GLIS_error_to_string_GL("glAttachShader");
    LOG_INFO("Attached fragment Shader to program");
    LOG_INFO("Linking Shader program");
    glLinkProgram(shaderProgram);
    GLIS_error_to_string_GL("glLinkProgram");
    LOG_INFO("Linked Shader program");
    LOG_INFO("Validating Shader program");
    GLboolean ProgramIsValid = GLIS_validate_program(shaderProgram);
    assert(ProgramIsValid == GL_TRUE);
    LOG_INFO("Validated Shader program");
}
