#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include <GL/glew.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

void GLIS_build_simple_shader_program(
    GLuint & vertexShader, const char *vertexSource,
    GLuint & fragmentShader, const char *fragmentSource,
    GLuint &shaderProgram
);

int LOG_INFO(const char* format, ... );
int LOG_ERROR(const char* format, ... );
void LOG_ALWAYS_FATAL(const char* format, ... );


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
    
    GLIS_build_simple_shader_program(
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
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, static_cast<GLfloat>(glutGet(GLUT_WINDOW_WIDTH)), 0.0f, static_cast<GLfloat>(glutGet(GLUT_WINDOW_HEIGHT)))));

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

	glutSwapBuffers();
}

void free_resources() {
    glDeleteProgram(program);
    glDeleteShader(f);
    glDeleteShader(v);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(2,0);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Texture atlas text");

	if (argc > 1)
		fontfilename = argv[1];
	else
		fontfilename = "FreeSans.ttf";

	GLenum glew_status = glewInit();

	if (GLEW_OK != glew_status) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "No support for OpenGL 2.0 found\n");
		return 1;
	}

	if (init_resources()) {
		glutDisplayFunc(display);
		glutMainLoop();
	}

	free_resources();
	return 0;
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
