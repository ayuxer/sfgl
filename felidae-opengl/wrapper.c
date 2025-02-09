#include "felidae/opengl/wrapper.h"
#include "felidae/opengl/graphics.h"
#include "felidae/windowing/core.h"

#include <EGL/egl.h>
#include <glad/gl.h>
#include <stddef.h>

felidae_gl_id vao, program_id;

void felidae_graphics_init(felidae_window_t *_, felidae_graphics_context_t *ctx)
{
    gladLoadGL((GLADloadfunc)eglGetProcAddress);
    eglSwapInterval(ctx->display, 1);

    felidae_shader_compilation_result shaders[2]
        = { felidae_create_shader(DEFAULT_VSHADER, VERTEX_SHADER),
            felidae_create_shader(DEFAULT_FSHADER, FRAGMENT_SHADER) };
    felidae_shader_compilation_result program
        = felidae_create_program(shaders, 2);

    float vertices[]
        = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };
    vao = felidae_generate_vao(true);
    felidae_generate_vbo(vertices, sizeof(vertices), STATIC_USAGE);
    felidae_add_vertex_attribute(0, 3, FLOAT, 3 * sizeof(float));
    program_id = program.data.id;
}

void felidae_graphics_start(
    felidae_window_t *window, felidae_graphics_context_t *_
)
{
    struct felidae_window_dimensions dimensions
        = felidae_get_window_dimensions(window);
    int width = dimensions.width, height = dimensions.height;
    glViewport(0, 0, width, height);
}

void felidae_graphics_end(felidae_window_t *_, felidae_graphics_context_t *ctx)
{
    glClear(GL_COLOR_BUFFER_BIT);
    felidae_use_program(program_id);
    felidae_bind_vao(vao);
    felidae_draw_vertices(TRIANGLES, 0, 3);
    eglSwapBuffers(ctx->display, ctx->surface);
}

void DrawBackground(
    unsigned int red, unsigned int green, unsigned int blue, float alpha
)
{
    glClearColor(red / 255., green / 255., blue / 255., alpha);
}
