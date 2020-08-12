#include <glis/internal/internal.hpp>
#include <glis/backup/backup.hpp>

void GLIS_FONT::set_max_width_height(GLint width, GLint height) {
    this->width = width;
    this->height = height;
}

void GLIS_FONT::set_max_width_height(GLIS_CLASS &screen) {
    this->width = screen.width;
    this->height = screen.height;
}

GLIS_FONT::ATLAS_TYPE * GLIS_FONT::add_font(const char *id, const char *path) {
    font_init *i = new font_init(path);
    i->load();
    Object * o = font.newObject(
            0,
            0,
            ATLAS_TYPE{
                    id,
                    path,
                    AnyOpt(i, true),
                    AnyOpt(new font_data(Kernel()), true)
            }
    );
    return o == nullptr ? nullptr : o->resource.get<ATLAS_TYPE*>();
}

Object *GLIS_FONT::find_font(const char *id) {
    size_t resource_len = strlen(id);
    int page = 1;
    size_t index = 0;
    size_t page_size = font.table->page_size;
    printf("font.table->Page.count() is %d\n", font.table->Page.count());
    for (; page <= font.table->Page.count(); page++) {
        index = ((page_size * page) - page_size);
        for (; index < page_size * page; index++)
            if (font.table->table[index] != nullptr) {
                const char *data = font.table->table[index]->resource.get<ATLAS_TYPE *>()->first;
                if (!memcmp(data, id, resource_len))
                    return font.table->table[index];
            } else {
                printf("font.table->table[%zu] is nullptr\n", index);
            }
    }
    return nullptr;
}

GLIS_FONT::ATLAS_TYPE *GLIS_FONT::get_atlas(const char *id) {
    auto o = find_font(id);
    if (o == nullptr) return nullptr;
    return o->resource.get<ATLAS_TYPE *>();
}

GLIS_FONT::atlas * GLIS_FONT::add_font_size(const char *id, int size) {
    ATLAS_TYPE *data = get_atlas(id);
    if (data == nullptr) return nullptr;
    auto fontInit = data->third.get<font_init *>();
    auto fontData = data->fourth.get<font_data *>();
    atlas x = atlas();
    x.init(fontInit->face, size);
    x.font_source = fontInit;
    Object * o = fontData->sizes.newObject(0, 0, x);
    return o == nullptr ? nullptr : o->resource.get<atlas*>();
}

GLIS_FONT::atlas *GLIS_FONT::find_size(const char *id, GLIS_FONT::font_data *fontData, int size) {
    Kernel *x = &fontData->sizes;
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
                        atlas *atlas_ = x->table->table[index]->resource.get<atlas *>();
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

GLIS_FONT::atlas *GLIS_FONT::find_size(const char *id, int size) {
    ATLAS_TYPE *data = get_atlas(id);
    if (data == nullptr) return nullptr;
    font_data *fontData = data->fourth.get<font_data *>();
    assert(fontData != nullptr);
    return find_size(id, fontData, size);
}

void GLIS_FONT::set_color(const GLfloat color[4]) {
    current_color = const_cast<GLfloat *>(color);
}

void
GLIS_FONT::render_text(const char *text, GLIS_FONT::atlas *a, float x, float y, float sx, float sy,
        const GLfloat * color) {
    GLIS_BACKUP backup;

    backup.program.backup();

    glUseProgram(a->font_source->program);

    /* Enable blending, necessary for our alpha texture */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (color != nullptr) {
        glUniform4fv(a->font_source->uniform_color, 1, color);
    } else if (current_color != nullptr) {
        glUniform4fv(a->font_source->uniform_color, 1, current_color);
    } else {
        glUniform4fv(a->font_source->uniform_color, 1, colors.black);
    }

    // flip y axis
    float y_ = static_cast<float>(height) - y;

    printf("text: %s\n", text);
    printf("x: %G, y: %G\n", x, y);
    printf("sx: %G, sy: %G\n", 1.0f, 1.0f);
    const uint8_t *p;

    /* Use the texture containing the atlas */
    glBindTexture(GL_TEXTURE_2D, a->tex);
    glUniform1i(a->font_source->uniform_tex, 0);
    glUniformMatrix4fv(
            glGetUniformLocation(a->font_source->program, "projection"),
            1,
            GL_FALSE,
           glm::value_ptr(
                   glm::ortho(
                           0.0f,
                           static_cast<GLfloat>(width),
                           0.0f,
                                static_cast<GLfloat>(height)
                   )
           )
    );

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(a->font_source->attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, a->font_source->vbo);
    glVertexAttribPointer(a->font_source->attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    typedef font_init::point point;

    point coords[6 * strlen(text)];
    int c = 0;

    /* Loop through all characters */
    for (p = (const uint8_t *) text; *p; p++) {
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
                x2 + w, -y2 - h, a->c[*p].tx + a->c[*p].bw / a->w,
                a->c[*p].ty + a->c[*p].bh / a->h};
    }

    /* Draw all the character on the screen in one go */
    glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, c);
    glDisableVertexAttribArray(a->font_source->attribute_coord);
    glDisable(GL_BLEND);
    backup.program.restore();
}

void
GLIS_FONT::render_text(const char *text, GLIS_FONT::atlas *a, float x, float y, float sx, float sy)
{
    render_text(text, a, x, y, sx, sy, nullptr);
}

void
GLIS_FONT::render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                       float sx, float sy, const GLfloat * color) {
    ATLAS_TYPE *data = get_atlas(font_id);
    if (data == nullptr) return;
    atlas *a = find_size(font_id, data->fourth.get<font_data *>(), font_size);
    render_text(text, a, x, y, sx, sy, color);
}

void
GLIS_FONT::render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                       float sx, float sy) {
    render_text(text, font_id, font_size, x, y, sx, sy, nullptr);
}

void
GLIS_FONT::render_text(const char *text, GLIS_FONT::atlas *a, float x, float y,
        const GLfloat * color) {
    render_text(text, a, x, y, 1.0f, 1.0f, color);
}

void
GLIS_FONT::render_text(const char *text, GLIS_FONT::atlas *a, float x, float y) {
    render_text(text, a, x, y, 1.0f, 1.0f, nullptr);
}

void
GLIS_FONT::render_text(const char *text, const char *font_id, const int font_size, float x, float y,
        const GLfloat * color) {
    render_text(text, font_id, font_size, x, y, 1.0f, 1.0f, color);
}

void
GLIS_FONT::render_text(const char *text, const char *font_id, const int font_size, float x, float y)
{
    render_text(text, font_id, font_size, x, y, 1.0f, 1.0f, nullptr);
}

void
GLIS_FONT::render_text(const char *text, GLIS_FONT::atlas *a, float x, float y, float sxy,
                       const GLfloat * color) {
    render_text(text, a, x, y, sxy, sxy, color);
}

void
GLIS_FONT::render_text(const char *text, GLIS_FONT::atlas *a, float x, float y, float sxy) {
    render_text(text, a, x, y, sxy, sxy, nullptr);
}

void
GLIS_FONT::render_text(const char *text, const char *font_id, const int font_size, float x, float y,
                       float sxy, const GLfloat * color) {
    render_text(text, font_id, font_size, x, y, sxy, sxy, color);
}

void
GLIS_FONT::render_text(const char *text, const char *font_id, const int font_size, float x, float y,
        float sxy)
{
    render_text(text, font_id, font_size, x, y, sxy, sxy, nullptr);
}

GLIS_FONT::font_init::font_init() {
    std::cout << "font_init default constructor" << std::endl << std::flush;
}

GLIS_FONT::font_init::font_init(const char *path) {
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

GLIS_FONT::font_init::font_init(const GLIS_FONT::font_init &p2) {
    std::cout << "font_init copy constructor" << std::endl << std::flush;
    this->font_path = p2.font_path;
    this->ft_initialized = p2.ft_initialized;
    this->ft = p2.ft;
    this->face = p2.face;
    this->hasProgram = p2.hasProgram;
    std::swap(this->v, const_cast<font_init &>(p2).v);
    std::swap(this->f, const_cast<font_init &>(p2).f);
    std::swap(this->program, const_cast<font_init &>(p2).program);
    std::swap(this->uniform_color, const_cast<font_init &>(p2).uniform_color);
    std::swap(this->uniform_tex, const_cast<font_init &>(p2).uniform_tex);
    std::swap(this->attribute_coord, const_cast<font_init &>(p2).attribute_coord);
    std::swap(this->vbo, const_cast<font_init &>(p2).vbo);
}

GLIS_FONT::font_init::font_init(GLIS_FONT::font_init &&p2) {
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

GLIS_FONT::font_init &GLIS_FONT::font_init::operator=(const GLIS_FONT::font_init &p2) {
    std::cout << "font_init copy assignment" << std::endl << std::flush;
    fflush(stdout);
    this->font_path = p2.font_path;
    this->ft_initialized = p2.ft_initialized;
    this->ft = p2.ft;
    this->face = p2.face;
    this->hasProgram = p2.hasProgram;
    std::swap(this->v, const_cast<font_init &>(p2).v);
    std::swap(this->f, const_cast<font_init &>(p2).f);
    std::swap(this->program, const_cast<font_init &>(p2).program);
    std::swap(this->uniform_color, const_cast<font_init &>(p2).uniform_color);
    std::swap(this->uniform_tex, const_cast<font_init &>(p2).uniform_tex);
    std::swap(this->attribute_coord, const_cast<font_init &>(p2).attribute_coord);
    std::swap(this->vbo, const_cast<font_init &>(p2).vbo);
    return *const_cast<font_init *>(this);
}

GLIS_FONT::font_init &GLIS_FONT::font_init::operator=(GLIS_FONT::font_init &&p2) {
    std::cout << "font_init move assignment" << std::endl << std::flush;
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
    return *const_cast<font_init *>(this);
}

GLIS_FONT::font_init::~font_init() {
    std::cout << "font_init destructor" << std::endl << std::flush;
    if (hasProgram) {
        glDeleteProgram(program);
        glDeleteShader(f);
        glDeleteShader(v);
        hasProgram = false;
    }
}

bool GLIS_FONT::font_init::load() {
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

    GLIS glis;
    glis.GLIS_build_simple_shader_program(
            v, vs,
            f, fs,
            program
    );
    if (program == 0) return false;

    hasProgram = true;

    attribute_coord = glGetAttribLocation(program, "coord");
    uniform_tex = glGetUniformLocation(program, "tex");
    uniform_color = glGetUniformLocation(program, "color");

    if (attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
        return false;

    // Create the vertex buffer object
    glGenBuffers(1, &vbo);
    return true;
}

void GLIS_FONT::atlas::init(FT_Face face, int height) {
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

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA,
                        GL_UNSIGNED_BYTE, g->bitmap.buffer);
        c[i].ax = g->advance.x >> 6;
        c[i].ay = g->advance.y >> 6;

        c[i].bw = g->bitmap.width;
        c[i].bh = g->bitmap.rows;

        c[i].bl = g->bitmap_left;
        c[i].bt = g->bitmap_top;

        c[i].tx = ox / (float) w;
        c[i].ty = oy / (float) h;

        rowh = std::max(rowh, g->bitmap.rows);
        ox += g->bitmap.width + 1;
    }

    fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
}

void GLIS_FONT::atlas::denit() {
    glDeleteTextures(1, &tex);
}

GLIS_FONT::atlas::atlas(FT_Face face, int height) {
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

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_ALPHA,
                        GL_UNSIGNED_BYTE, g->bitmap.buffer);
        c[i].ax = g->advance.x >> 6;
        c[i].ay = g->advance.y >> 6;

        c[i].bw = g->bitmap.width;
        c[i].bh = g->bitmap.rows;

        c[i].bl = g->bitmap_left;
        c[i].bt = g->bitmap_top;

        c[i].tx = ox / (float) w;
        c[i].ty = oy / (float) h;

        rowh = std::max(rowh, g->bitmap.rows);
        ox += g->bitmap.width + 1;
    }

    fprintf(stderr, "Generated a %d x %d (%d kb) texture atlas\n", w, h, w * h / 1024);
}

GLIS_FONT::atlas::atlas() {
    std::cout << "atlas constructor" << std::endl << std::flush;
}

constexpr GLIS_FONT::atlas::atlas(const GLIS_FONT::atlas &p2) {
    std::cout << "atlas copy constructor" << std::endl << std::flush;
    this->size = p2.size;
    this->tex = p2.tex;
    this->w = p2.w;
    this->h = p2.h;
    this->font_source = p2.font_source;
    for (int i = 0; i != 128; i++) this->c[i] = p2.c[i];
}

constexpr GLIS_FONT::atlas::atlas(GLIS_FONT::atlas &&p2) {
    std::cout << "atlas move constructor" << std::endl << std::flush;
    std::swap(this->size, p2.size);
    std::swap(this->tex, p2.tex);
    std::swap(this->w, p2.w);
    std::swap(this->h, p2.h);
    std::swap(this->font_source, p2.font_source);
    for (int i = 0; i != 128; i++) std::swap(this->c[i], p2.c[i]);
}

GLIS_FONT::atlas &GLIS_FONT::atlas::operator=(const GLIS_FONT::atlas &p2) {
    std::cout << "atlas copy assignment" << std::endl << std::flush;
    this->size = p2.size;
    this->tex = p2.tex;
    this->w = p2.w;
    this->h = p2.h;
    this->font_source = p2.font_source;
    for (int i = 0; i != 128; i++) this->c[i] = p2.c[i];
    return *const_cast<atlas *>(this);
}

GLIS_FONT::atlas &GLIS_FONT::atlas::operator=(GLIS_FONT::atlas &&p2) {
    std::cout << "atlas move assignment" << std::endl << std::flush;
    std::swap(this->size, p2.size);
    std::swap(this->tex, p2.tex);
    std::swap(this->w, p2.w);
    std::swap(this->h, p2.h);
    std::swap(this->font_source, p2.font_source);
    for (int i = 0; i != 128; i++) std::swap(this->c[i], p2.c[i]);
    return *const_cast<atlas *>(this);
}

GLIS_FONT::atlas::~atlas() {
    std::cout << "atlas destructor" << std::endl << std::flush;
    denit();
}

GLIS_FONT::font_data::font_data() {
    std::cout << "font_data default constructor" << std::endl << std::flush;
}

GLIS_FONT::font_data::font_data(const Kernel &kernel) {
    std::cout << "font_data kernel constructor" << std::endl << std::flush;
    sizes = kernel;
}

GLIS_FONT::font_data::font_data(const GLIS_FONT::font_data &p2) {
    std::cout << "font_data copy constructor" << std::endl << std::flush;
    this->sizes = p2.sizes;
}

GLIS_FONT::font_data::font_data(GLIS_FONT::font_data &&p2) {
    std::cout << "font_data move constructor" << std::endl << std::flush;
    std::swap(this->sizes, p2.sizes);
}

GLIS_FONT::font_data &GLIS_FONT::font_data::operator=(const GLIS_FONT::font_data &p2) {
    std::cout << "font_data copy assignment" << std::endl << std::flush;
    this->sizes = p2.sizes;
    return *const_cast<font_data *>(this);
}

GLIS_FONT::font_data &GLIS_FONT::font_data::operator=(GLIS_FONT::font_data &&p2) {
    std::cout << "font_data move assignment" << std::endl << std::flush;
    std::swap(this->sizes, p2.sizes);
    return *const_cast<font_data *>(this);
}

GLIS_FONT::font_data::~font_data() {
    std::cout << "font_data destructor" << std::endl << std::flush;
}
