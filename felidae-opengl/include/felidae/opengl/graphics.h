#ifndef FELIDAE_OPENGL_GRAPHICS_H
#define FELIDAE_OPENGL_GRAPHICS_H

#include <glad/gl.h>

constexpr const char DEFAULT_VSHADER[]
    = "#version 460 core \n"
      "layout (location = 0) in vec3 vertexPosition; \n"
      "void main() { \n"
      "  gl_Position = vec4(vertexPosition.x, vertexPosition.y, "
      "vertexPosition.z, 1.0); \n"
      "} \n\0";

constexpr const char DEFAULT_FSHADER[]
    = "#version 460 core \n"
      "out vec4 FragColor; \n"
      "void main() { \n"
      "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); \n"
      "} \n\0";

typedef unsigned int felidae_gl_id;

/**
 * Buffer objects are contiguous blocks of memory allocated in VRAM. They enable
 * efficient data transfer by allowing large amounts of vertex data to be sent
 * to the graphics card in a single batch.
 *
 * Usage patterns can also be provided to hint the GPU on how it could manage
 * this memory suitably and efficiently.
 */
typedef felidae_gl_id felidae_vbo;

enum felidae_usage_hint {
    // Usage pattern GPU hint that indicates that the data is changed a lot and
    // used many times.
    DYNAMIC_USAGE = GL_DYNAMIC_DRAW,

    // Usage pattern GPU hint that indicates that the the data is set only once
    // and used many times.
    STATIC_USAGE = GL_STATIC_DRAW,

    // Usage pattern GPU hint that indicates that he data is set only once and
    // used at most a few times.
    STREAM_USAGE = GL_STREAM_DRAW,
};

// The data type of a vertex attribute.
enum felidae_vertex_attrib_data_type {
    BYTE = GL_BYTE,
    UBYTE = GL_UNSIGNED_BYTE,
    FLOAT = GL_FLOAT,
    SHORT = GL_SHORT,
    USHORT = GL_UNSIGNED_SHORT,
    INT = GL_INT,
    UINT = GL_UNSIGNED_INT,
};

/**
 * A Shader is a user-defined program designed to run on some stage of a
 * graphics processor. Shaders provide the code for certain programmable
 * stages of the rendering pipeline. They can also be used in a slightly
 * more limited form for general, on-GPU computation.
 *
 * Source: https://www.khronos.org/opengl/wiki/Shader
 */
union felidae_shader_compilation_data {
    felidae_gl_id id;
    char error_message[512];
};

typedef struct {
    bool success;
    union felidae_shader_compilation_data data;
} felidae_shader_compilation_result;

enum felidae_shader_type {
    // A shader that processes rasterized vertices into a set of colors and a
    // single depth value.
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,

    // A shader that handles the processing and transformation of vertices.
    VERTEX_SHADER = GL_VERTEX_SHADER,
};

/**
 * OpenGL decides how to draw a set of vertices given a primitive type, the
 * starting index, and the amount of vertices.
 */
enum felidae_primitive_kind {
    POINTS = GL_POINTS,
    LINE_STRIP = GL_LINE_STRIP,
    LINE_LOOP = GL_LINE_LOOP,
    LINES = GL_LINES,
    LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
    LINES_ADJACENCY = GL_LINES_ADJACENCY,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    TRIANGLE_FAN = GL_TRIANGLE_FAN,
    TRIANGLES = GL_TRIANGLES,
    TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
    TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
    PATCHES = GL_PATCHES
};

/**
 * Generates a single vertex buffer object.
 */
felidae_vbo felidae_generate_vbo(
    const void *data, unsigned int len, enum felidae_usage_hint usage_hint
);

/**
 * Creates a vertex or fragment shader.
 */
felidae_shader_compilation_result
felidae_create_shader(const char *data, enum felidae_shader_type shader_type);

/**
 * A shader program is the final compiled version of a set of shaders. This
 * function expects a linked list of shader compilation results and its length
 * as arguments.
 */
felidae_shader_compilation_result felidae_create_program(
    felidae_shader_compilation_result shaders[], unsigned int shaders_len
);

void felidae_use_program(felidae_gl_id id);
void felidae_delete_program(felidae_gl_id id);

/**
 * Vertex attributes work like context parameters to vertex shaders.
 * Arguments:
 * - The position in the attribute layout;
 * - The amount of components, e.g. 1 for single elements and 2+ for arrays;
 * - The data type of the attribute;
 * - The whole length of the vertex attribute structure,
 *   usually given by `<component count> * sizeof(<intended data type>)`;
 */
void felidae_add_vertex_attribute(
    unsigned int layout_index, unsigned int count,
    enum felidae_vertex_attrib_data_type data_type, unsigned int len
);

/**
 * A vertex array object is essentially a set of VBOs. Instead of (un)binding
 * each buffer each time, it's required to use a VAO instead. You can think of
 * a VAO like a VBO supervisor.

 */
void felidae_bind_vao(felidae_gl_id id);

/**
 * Arguments:
 * - Whether to immediately bind it after creation.
 */
felidae_gl_id felidae_generate_vao(bool bind);

/**
 * OpenGL decides how to draw a set of vertices given a primitive type, the
 * starting index, and the amount of vertices.
 */
void felidae_draw_vertices(
    enum felidae_primitive_kind primitive_kind, int starting_index,
    int vertice_count
);

#endif
