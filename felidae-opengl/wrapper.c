#include "felidae/opengl/wrapper.h"
#include "felidae/common/payload.h"
#include "felidae/opengl/graphics.h"
#include "felidae/windowing/core.h"

#include <EGL/egl.h>
#include <glad/gl.h>
#include <stddef.h>

struct felidae_graphics_wrapper_state {
    felidae_gl_id program, vao, vbo;
};

static struct felidae_graphics_wrapper_state GRAPHICS = { 0 };

felidae_payload_result compile_default_program(felidae_gl_id *out)
{
    felidae_shader_compilation_result shaders[2]
        = { felidae_create_shader(DEFAULT_VSHADER, VERTEX_SHADER),
            felidae_create_shader(DEFAULT_FSHADER, FRAGMENT_SHADER) };
    auto res = felidae_create_program(shaders, 2);
    if (!res.success)
        return felidae_make_payload_result(
            FAILED_TO_BIND_OPENGL, res.data.error_message
        );
    *out = res.data.id;
    return felidae_success();
}

felidae_payload_result
felidae_graphics_init(felidae_window_t *_, felidae_graphics_context_t *ctx)
{
    gladLoadGL((GLADloadfunc)eglGetProcAddress);
    eglSwapInterval(ctx->display, 1);
    auto err = compile_default_program(&GRAPHICS.program);
    if (err.kind)
        return err;
    float vertices[] = {
        // positions         // colors
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f // top
    };
    GRAPHICS.vao = felidae_generate_vao(true);
    GRAPHICS.vbo
        = felidae_generate_vbo(vertices, sizeof(vertices), STATIC_USAGE);
    felidae_add_vertex_attribute(0, 3, FLOAT, 6 * sizeof(float), 0);
    felidae_add_vertex_attribute(
        1, 3, FLOAT, 6 * sizeof(float), 3 * sizeof(float)
    );
    felidae_name_vertex_attribute(GRAPHICS.program, 0, "vertexPosition");
    felidae_name_vertex_attribute(GRAPHICS.program, 1, "vertexColor");
    felidae_unbind_vbo();
    felidae_unbind_vao();
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
}

void felidae_graphics_end(
    felidae_window_t *window, felidae_graphics_context_t *ctx
)
{
    glClear(GL_COLOR_BUFFER_BIT);
    felidae_use_program(GRAPHICS.program);
    felidae_bind_vao(GRAPHICS.vao);
    felidae_draw_vertices(TRIANGLES, 0, 6);
    eglSwapBuffers(ctx->display, ctx->surface);
}

void felidae_graphics_cleanup(
    felidae_window_t *window, felidae_graphics_context_t *ctx
)
{
    felidae_delete_vao(GRAPHICS.vao);
    felidae_delete_buffer(GRAPHICS.vbo);
    felidae_delete_program(GRAPHICS.program);
}

void DrawBackground(
    unsigned int red, unsigned int green, unsigned int blue, float alpha
)
{
    glClearColor(red / 255., green / 255., blue / 255., alpha);
}
