#include "felidae/opengl/batch.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static felidae_payload_result compile_default_program(felidae_gl_id *out)
{
    felidae_shader_compilation_result shaders[2]
        = { felidae_create_shader(DEFAULT_VSHADER, VERTEX_SHADER),
            felidae_create_shader(DEFAULT_FSHADER, FRAGMENT_SHADER) };

    auto result = felidae_create_program(shaders, 2);
    if (!result.success)
        return felidae_make_payload_result(
            FAILED_TO_COMPILE_SHADERS, result.data.error_message
        );

    *out = result.data.id;
    return felidae_success();
}

felidae_payload_result felidae_init_batch_renderer(
    felidae_batch_renderer *renderer, unsigned int buffer_capacity
)
{
    auto result = compile_default_program(&renderer->shader_program);
    if (result.kind)
        return result;

    renderer->vao = felidae_generate_vao(true);
    renderer->vbo = felidae_generate_vbo(
        NULL, sizeof(felidae_vertex) * buffer_capacity, DYNAMIC_USAGE
    );

    felidae_add_vertex_attribute(
        0, 3, FLOAT, sizeof(felidae_vertex),
        offsetof(felidae_vertex, vertex_position)
    );
    felidae_add_vertex_attribute(
        1, 4, FLOAT, sizeof(felidae_vertex),
        offsetof(felidae_vertex, vertex_color)
    );
    felidae_add_vertex_attribute(
        2, 2, FLOAT, sizeof(felidae_vertex),
        offsetof(felidae_vertex, texture_coordinates)
    );
    felidae_name_vertex_attribute(
        renderer->shader_program, 0, "vertexPosition"
    );
    felidae_name_vertex_attribute(renderer->shader_program, 1, "vertexColor");
    felidae_name_vertex_attribute(
        renderer->shader_program, 2, "vertexTexCoord"
    );

    renderer->buf = (felidae_vertex_buffer
    ) { .capacity = buffer_capacity,
        .len = 0,
        .data = malloc(sizeof(felidae_vertex) * buffer_capacity) };

    renderer->uniforms = (felidae_uniforms) { 0 };
    renderer->uniforms.texture_location
        = glGetUniformLocation(renderer->shader_program, "texture0");
    renderer->uniforms.mvp_location
        = glGetUniformLocation(renderer->shader_program, "mvp");

    return felidae_success();
}

void felidae_flush_vertices(felidae_batch_renderer *renderer)
{
    felidae_use_program(renderer->shader_program);
    felidae_bind_texture(FIRST_UNIT, renderer->uniforms.texture);

    glUniform1i(renderer->uniforms.texture_location, 0);

    glUniformMatrix4fv(
        renderer->uniforms.mvp_location, 1, GL_FALSE,
        renderer->uniforms.mvp.columns[0]
    );

    felidae_bind_vbo(renderer->vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(felidae_vertex) * renderer->buf.len,
        renderer->buf.data
    );
    felidae_bind_vao(renderer->vao);

    felidae_draw_vertices(TRIANGLES, 0, renderer->buf.len);
    renderer->buf.len = 0;
}

void felidae_set_texture(
    felidae_batch_renderer *renderer, felidae_gl_id texture
)
{
    if (renderer->uniforms.texture == texture)
        return;
    felidae_flush_vertices(renderer);
    renderer->uniforms.texture = texture;
}

void felidae_set_mvp(felidae_batch_renderer *renderer, felidae_matrix matrix)
{
    int comparison = memcmp(
        &renderer->uniforms.mvp.columns, &matrix.columns, sizeof(felidae_matrix)
    );
    if (comparison == 0)
        return;
    felidae_flush_vertices(renderer);
    renderer->uniforms.mvp = matrix;
}

void felidae_push_vertex(
    felidae_batch_renderer *renderer, float x, float y, float z, float red,
    float green, float blue, float alpha, float u, float v
)
{
    if (renderer->buf.len == renderer->buf.capacity)
        felidae_flush_vertices(renderer);
    renderer->buf.data[renderer->buf.len++]
        = (felidae_vertex) { .vertex_position = { x, y, z },
                             .texture_coordinates = { u, v },
                             .vertex_color = { red, green, blue, alpha } };
}
