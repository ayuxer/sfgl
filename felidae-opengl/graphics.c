#include <glad/gl.h>
#include <stddef.h>
#include <stdio.h>

#include "felidae/opengl/graphics.h"

felidae_shader_compilation_result
felidae_create_shader(const char *data, enum felidae_shader_type shader_type)
{
    felidae_gl_id shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    union felidae_shader_compilation_data output;
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

felidae_shader_compilation_result felidae_create_program(
    felidae_shader_compilation_result shaders[], unsigned int shaders_len
)
{
    felidae_gl_id program = glCreateProgram();
    for (unsigned int index = 0; index < shaders_len; index++) {
        felidae_shader_compilation_result shader = shaders[index];
        if (shader.success)
            glAttachShader(program, shader.data.id);
    }
    glLinkProgram(program);
    int success;
    union felidae_shader_compilation_data output;
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

void felidae_delete_program(felidae_gl_id id) { glDeleteShader(id); }

void felidae_add_vertex_attribute(
    unsigned int layout_index, unsigned int count,
    enum felidae_vertex_attrib_data_type data_type, unsigned int len
)
{
    glVertexAttribPointer(
        layout_index, count, data_type, GL_FALSE, len, (void *)0
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

void felidae_draw_vertices(
    enum felidae_primitive_kind primitive_kind, int starting_index,
    int vertice_count
)
{
    glDrawArrays(primitive_kind, starting_index, vertice_count);
}
