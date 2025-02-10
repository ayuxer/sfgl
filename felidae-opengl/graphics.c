#include <glad/gl.h>
#include <stddef.h>
#include <stdio.h>

#include "felidae/opengl/graphics.h"
#include <stb/image.h>
#include <string.h>

felidae_shader_compilation_result
felidae_create_shader(const char *data, enum felidae_shader_type shader_type)
{
    felidae_gl_id shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    felidae_shader_compilation_data output;
    if (!success)
        glGetShaderInfoLog(shader, 512, NULL, output.error_message);
    else
        output.id = shader;
    return (felidae_shader_compilation_result) { .success = success,
                                                 .data = output };
}

felidae_vbo felidae_generate_vbo(
    const void *data, unsigned int len, enum felidae_usage_hint usage_hint
)
{
    felidae_gl_id vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, len, data, usage_hint);
    return vbo;
}

felidae_vbo felidae_generate_ebo(
    const void *data, unsigned int len, enum felidae_usage_hint usage_hint
)
{
    felidae_gl_id ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, len, data, usage_hint);
    return ebo;
}

felidae_shader_compilation_result felidae_create_program(
    felidae_shader_compilation_result shaders[], unsigned int shaders_len
)
{
    felidae_gl_id program = glCreateProgram();
    for (unsigned int index = 0; index < shaders_len; index++) {
        felidae_shader_compilation_result shader = shaders[index];
        if (!shader.success)
            return shader;
        glAttachShader(program, shader.data.id);
    }
    glLinkProgram(program);
    int success;
    felidae_shader_compilation_data output;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
        glGetProgramInfoLog(program, 512, NULL, output.error_message);
    else
        output.id = program;
    for (unsigned int index = 0; index < shaders_len; index++) {
        felidae_shader_compilation_result shader = shaders[index];
        if (shader.success)
            glDeleteShader(shader.data.id);
    }
    return (felidae_shader_compilation_result) { .success = success,
                                                 .data = output };
}

void felidae_use_program(felidae_gl_id id) { glUseProgram(id); }

void felidae_delete_program(felidae_gl_id id) { glDeleteProgram(id); }
void felidae_delete_buffer(felidae_gl_id id) { glDeleteBuffers(1, &id); }
void felidae_delete_vao(felidae_gl_id id) { glDeleteVertexArrays(1, &id); }

void felidae_name_vertex_attribute(
    felidae_gl_id program, unsigned int layout_index, const char *name
)
{
    glBindAttribLocation(program, layout_index, name);
}

void felidae_add_vertex_attribute(
    unsigned int layout_index, unsigned int count,
    enum felidae_vertex_attrib_data_type data_type, unsigned int len, int offset
)
{
    glVertexAttribPointer(
        layout_index, count, data_type, GL_FALSE, len, (void *)offset
    );
    glEnableVertexAttribArray(layout_index);
}

felidae_gl_id felidae_generate_vao(bool bind)
{
    felidae_gl_id vao;
    glGenVertexArrays(1, &vao);
    if (bind)
        felidae_bind_vao(vao);
    return vao;
}

void felidae_bind_vao(felidae_gl_id id) { glBindVertexArray(id); }
void felidae_bind_vbo(felidae_gl_id id) { glBindBuffer(GL_ARRAY_BUFFER, id); }

void felidae_draw_vertices(
    enum felidae_primitive_kind primitive_kind, int starting_index,
    int vertice_count
)
{
    glDrawArrays(primitive_kind, starting_index, vertice_count);
}

void felidae_draw_vertices_indexed(
    enum felidae_primitive_kind primitive_kind, int vertice_count
)
{
    glDrawElements(primitive_kind, vertice_count, GL_UNSIGNED_INT, 0);
}

void felidae_polygon_mode(enum felidae_polygon_mode_type mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, mode);
}

void felidae_unbind_vao() { glBindVertexArray(0); }
void felidae_unbind_vbo() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
void felidae_unbind_ebo() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

struct felidae_gl_result felidae_load_texture(
    char *path, unsigned int mipmap_level, enum felidae_texture_wrap_mode wrap,
    enum felidae_texture_filter_mode filter
)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    struct felidae_gl_result result;
    if (!data) {
        result.success = false;
        strcpy(result.data.error_message, "Failed to decode image.");
        return result;
    }

    felidae_gl_id texture;
    glGenTextures(1, &texture);
    glBindTexture(TWO_DIMENSIONAL, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage2D(
        TWO_DIMENSIONAL, mipmap_level, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(TWO_DIMENSIONAL);

    stbi_image_free(data);
    result.success = true;
    result.data.id = texture;

    return result;
}

void felidae_bind_texture(enum felidae_texture_unit unit, felidae_gl_id texture)
{
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}
