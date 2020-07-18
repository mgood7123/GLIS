//
// Created by smallville7123 on 12/07/20.
//

#pragma once

template<typename TYPE> float GLIS_inverse(TYPE num) {
    return num < 0 ? -num : -(num);
}

template<typename TYPE> float GLIS_convert(TYPE num, TYPE num_max) {
    // 3 year old magic
    return (num - (num_max / 2)) / (num_max / 2);
}

template<typename TYPE> class GLIS_coordinates {
public:
    GLIS_coordinates() {}
    GLIS_coordinates(TYPE TYPE_INITIALIZER) {
        TYPE x = TYPE_INITIALIZER;
        TYPE y = TYPE_INITIALIZER;
    }

    TYPE x;
    TYPE y;
};

template<typename TYPE> class GLIS_rect {
public:
    GLIS_rect(TYPE TYPE_INITIALIZER) {
        topLeft = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
        topRight = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
        bottomLeft = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
        bottomRight = GLIS_coordinates<TYPE>(TYPE_INITIALIZER);
    }

    GLIS_coordinates<TYPE> topLeft;

    GLIS_coordinates<TYPE> topRight;

    GLIS_coordinates<TYPE> bottomLeft;

    GLIS_coordinates<TYPE> bottomRight;
};

template<typename TYPE> class GLIS_rect<TYPE>
GLIS_points_to_rect(TYPE TYPE_INITIALIZER, TYPE x1, TYPE y1, TYPE x2, TYPE y2) {
    class GLIS_rect<TYPE> r(TYPE_INITIALIZER);
    r.bottomLeft.x = x1;
    r.bottomLeft.y = y1;
    r.topRight.x = x2;
    r.topRight.y = y2;
    r.bottomRight.x = x2;
    r.bottomRight.y = y1;
    r.topLeft.x = x1;
    r.topLeft.y = y2;
    return r;
}

#define GLIS_CONVERSION_ORIGIN_TOP_LEFT 0
#define GLIS_CONVERSION_ORIGIN_TOP_RIGHT 1
#define GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT 2
#define GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT 3

int GLIS_CONVERSION_ORIGIN = GLIS_CONVERSION_ORIGIN_TOP_LEFT;

void GLIS_set_conversion_origin(int origin);

template<typename TYPEFROM, typename TYPETO>
class GLIS_coordinates<TYPETO>
GLIS_convertPair(TYPETO TYPETO_INITIALIZER, TYPEFROM x, TYPEFROM y, TYPEFROM x_max,
                 TYPEFROM y_max, bool clip) {
    class GLIS_coordinates<TYPETO> xy(TYPETO_INITIALIZER);
    if (x > x_max) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("x is out of bounds (expected %hi, got %hi)", x_max, x);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", x_max);
            x = x_max;
        }
    } else if (x < 0) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("x is out of bounds (expected %hi, got %hi)", 0, x);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", 0);
            x = 0;
        }
    }
    if (y > y_max) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("y is out of bounds (expected %hi, got %hi)", y_max, y);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", y_max);
            y = y_max;
        }
    } else if (y < 0) {
        if (GLIS_LOG_PRINT_CONVERSIONS)
            LOG_INFO("y is out of bounds (expected %hi, got %hi)", 0, y);
        if (clip) {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("clipping to %hi", 0);
            y = 0;
        }
    }
    xy.x = GLIS_convert<TYPETO>(static_cast<TYPETO>(x), static_cast<TYPETO>(x_max)); // x
    xy.y = GLIS_convert<TYPETO>(static_cast<TYPETO>(y), static_cast<TYPETO>(y_max)); // y
    switch (GLIS_CONVERSION_ORIGIN) {
        case GLIS_CONVERSION_ORIGIN_TOP_LEFT:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'y'");
            xy.y = GLIS_inverse<TYPETO>(xy.y);
            break;
        case GLIS_CONVERSION_ORIGIN_TOP_RIGHT:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'x'");
            xy.x = GLIS_inverse<TYPETO>(xy.x);
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'y'");
            xy.y = GLIS_inverse<TYPETO>(xy.y);
            break;
        case GLIS_CONVERSION_ORIGIN_BOTTOM_LEFT: {
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("no conversion");
            break;
        }
        case GLIS_CONVERSION_ORIGIN_BOTTOM_RIGHT:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("inverting 'x'");
            xy.x = GLIS_inverse<TYPETO>(xy.x);
            break;
        default:
            if (GLIS_LOG_PRINT_CONVERSIONS) LOG_INFO("unknown conversion");
            break;
    }
    if (GLIS_LOG_PRINT_CONVERSIONS)
        LOG_INFO(
                "width: %hi, width_max: %hi, height: %hi, height_max: %hi, ConvertPair: %f, %f",
                x, x_max, y, y_max, xy.x, xy.y);
    return xy;
}

template<typename TYPEFROM, typename TYPETO>
class GLIS_coordinates<TYPETO>
GLIS_convertPair(TYPETO TYPETO_INITIALIZER, TYPEFROM x, TYPEFROM y, TYPEFROM x_max,
                 TYPEFROM y_max) {
    return GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER, x, y, x_max, y_max, true);
}

template<typename TYPE>
struct GLIS_quater_position {
    TYPE x;
    TYPE y;
    TYPE z;
};

template<typename TYPE>
struct GLIS_quater_color {
    TYPE R;
    TYPE G;
    TYPE B;
};

template<typename TYPE>
struct GLIS_quater {
    struct GLIS_quater_position<TYPE> position;
    struct GLIS_quater_color<TYPE> color;
    struct GLIS_quater_position<TYPE> texture_position;
};

template<typename TYPE>
struct GLIS_vertex_map_rectangle {
    struct GLIS_quater<TYPE> top_right;
    struct GLIS_quater<TYPE> bottom_right;
    struct GLIS_quater<TYPE> bottom_left;
    struct GLIS_quater<TYPE> top_left;
};

template<typename TYPE>
class GLIS_vertex_data {
public:
    TYPE *vertex;
    int size_of_position;
    int size_of_color;
    int size_of_texture_position;
    int size_per_quater;
    int number_of_points;
    size_t vertex_size;
    unsigned int *indices;
    size_t indices_size;
    size_t typesize;

    void print(const char *format) {
        std::string fmt = "\n";
        fmt += "VECTOR OUTPUT:    |       positions       |       colors          |texture positions|";
        fmt += "\n";
        fmt += "VALUES:           |   X      Y      Z     |   R      B      G     |   X      Y      |";
        fmt += "\n";
        fmt += "TOP RIGHT:      ";
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "   |";
        fmt += "\n";
        fmt += "BOTTOM RIGHT:   ";
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "   |";
        fmt += "\n";
        fmt += "BOTTOM LEFT:    ";
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "   |";
        fmt += "\n";
        fmt += "TOP LEFT:       ";
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "  |  ";
        fmt += format;
        fmt += ", ";
        fmt += format;
        fmt += "   |";
        LOG_INFO(fmt.c_str(),
                 vertex[0], vertex[1], vertex[2],
                 vertex[3], vertex[4], vertex[5],
                 vertex[6], vertex[7],
                 vertex[8], vertex[9], vertex[10],
                 vertex[11], vertex[12], vertex[13],
                 vertex[14], vertex[15],
                 vertex[16], vertex[17], vertex[18],
                 vertex[19], vertex[20], vertex[21],
                 vertex[22], vertex[23],
                 vertex[24], vertex[25], vertex[26],
                 vertex[27], vertex[28], vertex[29],
                 vertex[30], vertex[31]
        );
    }

    void init_attributes() {
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                              (void *) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
};

template<typename TYPE>
void GLIS_fill_vertex_rect(TYPE *vertex, struct GLIS_quater<TYPE> &quater, int offset) {
    vertex[offset + 0] = quater.position.x;
    vertex[offset + 1] = quater.position.y;
    vertex[offset + 2] = quater.position.z;
    vertex[offset + 3] = quater.color.R;
    vertex[offset + 4] = quater.color.G;
    vertex[offset + 5] = quater.color.B;
    vertex[offset + 6] = quater.texture_position.x;
    vertex[offset + 7] = quater.texture_position.y;
};

template<typename TYPE>
struct GLIS_vertex_data<TYPE>
GLIS_build_vertex_rect(struct GLIS_vertex_map_rectangle<TYPE> &data) {
    struct GLIS_vertex_data<TYPE> v;
    v.typesize = sizeof(TYPE);
    v.size_of_position = 3;
    v.size_of_color = 3;
    v.size_of_texture_position = 2;
    v.size_per_quater = v.size_of_position + v.size_of_color + v.size_of_texture_position;
    v.number_of_points = 4;
    v.vertex_size = v.typesize * (v.size_per_quater * v.number_of_points);
    v.vertex = static_cast<TYPE *>(malloc(v.vertex_size));
    GLIS_fill_vertex_rect(v.vertex, data.top_right, 0 * v.size_per_quater);
    GLIS_fill_vertex_rect(v.vertex, data.bottom_right, 1 * v.size_per_quater);
    GLIS_fill_vertex_rect(v.vertex, data.bottom_left, 2 * v.size_per_quater);
    GLIS_fill_vertex_rect(v.vertex, data.top_left, 3 * v.size_per_quater);
    v.indices_size = 6 * sizeof(unsigned int);
    v.indices = static_cast<unsigned int *>(malloc(v.indices_size));
    v.indices[0] = 0;
    v.indices[1] = 1;
    v.indices[2] = 3;
    v.indices[3] = 1;
    v.indices[4] = 2;
    v.indices[5] = 3;
    return v;
}

template<typename TYPEFROM, typename TYPETO>
struct GLIS_vertex_map_rectangle<TYPETO>
GLIS_build_vertex_data_rect(TYPETO TYPETO_INITIALIZER, class GLIS_rect<TYPEFROM> data,
                            TYPEFROM max_x, TYPEFROM max_y) {
    struct GLIS_vertex_map_rectangle<TYPETO> m;
    GLIS_coordinates<TYPETO> point1 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER,
                                                                         data.topLeft.x,
                                                                         data.topLeft.y, max_x,
                                                                         max_y);
    m.top_left.position.x = point1.x;
    m.top_left.position.y = point1.y;
    m.top_left.position.z = 0.0F;
    m.top_left.texture_position.x = 0.0F;
    m.top_left.texture_position.y = 1.0F;
    m.top_left.texture_position.z = 0.0F;
    m.top_left.color.R = 1.0F;
    m.top_left.color.G = 1.0F;
    m.top_left.color.B = 1.0F;
    GLIS_coordinates<TYPETO> point2 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER,
                                                                         data.topRight.x,
                                                                         data.topRight.y, max_x,
                                                                         max_y);
    m.top_right.position.x = point2.x;
    m.top_right.position.y = point2.y;
    m.top_right.position.z = 0.0F;
    m.top_right.texture_position.x = 1.0F;
    m.top_right.texture_position.y = 1.0F;
    m.top_right.texture_position.z = 0.0F;
    m.top_right.color.R = 0.0F;
    m.top_right.color.G = 1.0F;
    m.top_right.color.B = 0.0F;
    GLIS_coordinates<TYPETO> point3 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER,
                                                                         data.bottomLeft.x,
                                                                         data.bottomLeft.y,
                                                                         max_x, max_y);
    m.bottom_left.position.x = point3.x;
    m.bottom_left.position.y = point3.y;
    m.bottom_left.position.z = 0.0F;
    m.bottom_left.texture_position.x = 0.0F;
    m.bottom_left.texture_position.y = 0.0F;
    m.bottom_left.texture_position.z = 0.0F;
    m.bottom_left.color.R = 1.0F;
    m.bottom_left.color.G = 0.0F;
    m.bottom_left.color.B = 0.0F;
    GLIS_coordinates<TYPETO> point4 = GLIS_convertPair<TYPEFROM, TYPETO>(TYPETO_INITIALIZER,
                                                                         data.bottomRight.x,
                                                                         data.bottomRight.y,
                                                                         max_x, max_y);
    m.bottom_right.position.x = point4.x;
    m.bottom_right.position.y = point4.y;
    m.bottom_right.position.z = 0.0F;
    m.bottom_right.texture_position.x = 1.0F;
    m.bottom_right.texture_position.y = 0.0F;
    m.bottom_right.texture_position.z = 0.0F;
    m.bottom_right.color.R = 0.0F;
    m.bottom_right.color.G = 0.0F;
    m.bottom_right.color.B = 1.0F;
    return m;
}

template<typename TYPE>
void GLIS_draw_rectangle(TYPE INITIALIZER, TYPE x1, TYPE y1, TYPE x2, TYPE y2, TYPE max_x,
                         TYPE max_y) {
    class GLIS_rect<GLint> r = GLIS_points_to_rect<GLint>(INITIALIZER, x1, y1, x2, y2);
    struct GLIS_vertex_map_rectangle<float> vmr = GLIS_build_vertex_data_rect<GLint, float>(
            0.0F, r, max_x, max_y);
    class GLIS_vertex_data<float> v = GLIS_build_vertex_rect<float>(vmr);
    if (GLIS_LOG_PRINT_VERTEX) v.print("%4.1ff");

    GLuint vertex_array_object;
    GLuint vertex_buffer_object;
    GLuint element_buffer_object;
    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Generating buffers");
    glGenVertexArrays(1, &vertex_array_object);
    GLIS_error_to_string_GL("glGenVertexArrays");
    glGenBuffers(1, &vertex_buffer_object);
    GLIS_error_to_string_GL("glGenBuffers");
    glGenBuffers(1, &element_buffer_object);
    GLIS_error_to_string_GL("glGenBuffers");
    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Binding buffers");
    glBindVertexArray(vertex_array_object);
    GLIS_error_to_string_GL("glBindVertexArray");
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    GLIS_error_to_string_GL("glBindBuffer");
    glBufferData(GL_ARRAY_BUFFER, v.vertex_size, v.vertex, GL_STATIC_DRAW);
    GLIS_error_to_string_GL("glBufferData");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    GLIS_error_to_string_GL("glBindBuffer");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, v.indices_size, v.indices, GL_STATIC_DRAW);
    GLIS_error_to_string_GL("glBufferData");
    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Initializing Attributes");
    v.init_attributes();

    if (GLIS_LOG_PRINT_SHAPE_INFO) LOG_INFO("Drawing rectangle");
    glBindVertexArray(vertex_array_object);
    GLIS_error_to_string_GL("glBindVertexArray");
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    GLIS_error_to_string_GL("glDrawElements");
    glBindVertexArray(0);
    GLIS_error_to_string_GL("glBindVertexArray");
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLIS_error_to_string_GL("glBindBuffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    GLIS_error_to_string_GL("glBindBuffer");
    glDeleteVertexArrays(1, &vertex_array_object);
    GLIS_error_to_string_GL("glDeleteVertexArrays");
    glDeleteBuffers(1, &vertex_buffer_object);
    GLIS_error_to_string_GL("glDeleteBuffers");
    glDeleteBuffers(1, &element_buffer_object);
    GLIS_error_to_string_GL("glDeleteBuffers");
}

template<typename TYPE>
void GLIS_draw_rectangle(
        GLenum textureUnit, GLuint texture, TYPE INITIALIZER, TYPE x1,
        TYPE y1, TYPE x2, TYPE y2, TYPE max_x, TYPE max_y
) {
    GLIS_set_texture(textureUnit, texture);
    GLIS_draw_rectangle<TYPE>(INITIALIZER, x1, y1, x2, y2, max_x, max_y);
}