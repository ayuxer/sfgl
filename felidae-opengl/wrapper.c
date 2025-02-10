#include "felidae/opengl/wrapper.h"
#include "felidae/common/payload.h"
#include "felidae/opengl/graphics.h"
#include "felidae/windowing/core.h"

#include <EGL/egl.h>
#include <glad/gl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    float columns[4][4];
} felidae_matrix;

typedef struct {
    float vertex_position[3];
    float vertex_color[4];
    float texture_coordinates[2];
} felidae_vertex;

typedef struct {
    unsigned int len, capacity;
    felidae_vertex *data;
} felidae_vertex_buffer;

typedef struct {
    felidae_gl_id texture;
    felidae_matrix mvp;
    felidae_gl_id texture_location, mvp_location;
} felidae_uniforms;

typedef struct {
    felidae_gl_id shader_program;
    felidae_gl_id vao, vbo;
    felidae_vertex_buffer buf;
    felidae_uniforms uniforms;
} felidae_batch_renderer;

typedef struct {
    felidae_gl_id placeholder_texture;
} felidae_outer_state;

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

static felidae_batch_renderer renderer = { 0 };

static felidae_matrix ortholinear(
    float left, float right, float bottom, float top, float znear, float zfar
)
{
    felidae_matrix m = { 0 };
    float rl = 1.0f / (right - left);
    float tb = 1.0f / (top - bottom);
    float fn = -1.0f / (zfar - znear);
    m.columns[0][0] = 2.0f * rl;
    m.columns[1][1] = 2.0f * tb;
    m.columns[2][2] = 2.0f * fn;
    m.columns[3][0] = -(right + left) * rl;
    m.columns[3][1] = -(top + bottom) * tb;
    m.columns[3][2] = (zfar + znear) * fn;
    m.columns[3][3] = 1.0f;
    return m;
}

felidae_payload_result felidae_init_batch_renderer(unsigned int buffer_capacity)
{
    auto result = compile_default_program(&renderer.shader_program);
    if (result.kind)
        return result;

    renderer.vao = felidae_generate_vao(true);
    renderer.vbo = felidae_generate_vbo(
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
    felidae_name_vertex_attribute(renderer.shader_program, 0, "vertexPosition");
    felidae_name_vertex_attribute(renderer.shader_program, 1, "vertexColor");
    felidae_name_vertex_attribute(renderer.shader_program, 2, "vertexTexCoord");

    renderer.buf = (felidae_vertex_buffer
    ) { .capacity = buffer_capacity,
        .len = 0,
        .data = malloc(sizeof(felidae_vertex) * buffer_capacity) };

    renderer.uniforms = (felidae_uniforms) { 0 };
    renderer.uniforms.texture_location
        = glGetUniformLocation(renderer.shader_program, "texture0");
    renderer.uniforms.mvp_location
        = glGetUniformLocation(renderer.shader_program, "mvp");

    return felidae_success();
}

void felidae_flush_vertices()
{
    felidae_use_program(renderer.shader_program);
    felidae_bind_texture(FIRST_UNIT, renderer.uniforms.texture);

    glUniform1i(renderer.uniforms.texture_location, 0);

    glUniformMatrix4fv(
        renderer.uniforms.mvp_location, 1, GL_FALSE,
        renderer.uniforms.mvp.columns[0]
    );

    felidae_bind_vbo(renderer.vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER, 0, sizeof(felidae_vertex) * renderer.buf.len,
        renderer.buf.data
    );
    felidae_bind_vao(renderer.vao);

    felidae_draw_vertices(TRIANGLES, 0, renderer.buf.len);
    renderer.buf.len = 0;
}

void felidae_set_texture(felidae_gl_id texture)
{
    if (renderer.uniforms.texture == texture)
        return;
    felidae_flush_vertices();
    renderer.uniforms.texture = texture;
}

void felidae_set_mvp(felidae_matrix matrix)
{
    int comparison = memcmp(
        &renderer.uniforms.mvp.columns, &matrix.columns, sizeof(felidae_matrix)
    );
    if (comparison == 0)
        return;
    felidae_flush_vertices();
    renderer.uniforms.mvp = matrix;
}

void felidae_push_vertex(
    float x, float y, float z, float red, float green, float blue, float alpha,
    float u, float v
)
{
    if (renderer.buf.len == renderer.buf.capacity)
        felidae_flush_vertices();
    renderer.buf.data[renderer.buf.len++]
        = (felidae_vertex) { .vertex_position = { x, y, z },
                             .texture_coordinates = { u, v },
                             .vertex_color = { red, green, blue, alpha } };
}

felidae_payload_result
felidae_graphics_init(felidae_window_t *_, felidae_graphics_context_t *ctx)
{
    gladLoadGL((GLADloadfunc)eglGetProcAddress);
    eglSwapInterval(ctx->display, 1);
    felidae_init_batch_renderer(5 * 1024);
    return felidae_success();
}

void felidae_graphics_start(
    felidae_window_t *window, felidae_graphics_context_t *ctx
)
{
    struct felidae_window_dimensions dimensions
        = felidae_get_window_dimensions(window);
    int width = dimensions.width, height = dimensions.height;
    glViewport(0, 0, width, height);
    felidae_set_mvp(ortholinear(0, width, height, 0, -1, 1));
}

void felidae_graphics_cleanup(
    felidae_window_t *window, felidae_graphics_context_t *ctx
)
{
    felidae_delete_vao(renderer.vao);
    felidae_delete_buffer(renderer.vbo);
    felidae_delete_program(renderer.shader_program);
}

void DrawBackground(
    unsigned int red, unsigned int green, unsigned int blue, float alpha
)
{
    glClearColor(red / 255., green / 255., blue / 255., alpha);
}

void DrawTriangle(void)
{
    felidae_set_texture(renderer.uniforms.texture);
    felidae_push_vertex(-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1, 1.0f, 1.0f);
    felidae_push_vertex(0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1, 1.0f, 0.0f);
    felidae_push_vertex(-0, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1, 0.0f, 0.0f);
}

void felidae_graphics_end(
    felidae_window_t *window, felidae_graphics_context_t *ctx
)
{
    glClear(GL_COLOR_BUFFER_BIT);
    DrawTriangle();

    felidae_flush_vertices();
    eglSwapBuffers(ctx->display, ctx->surface);
}
