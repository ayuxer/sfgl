#include "felidae/opengl/wrapper.h"
#include "cglm/cam.h"
#include "cglm/mat4.h"
#include "cglm/types.h"
#include "cglm/vec3.h"
#include "felidae/common/payload.h"
#include "felidae/opengl/batch.h"
#include "felidae/opengl/graphics.h"
#include "felidae/windowing/core.h"

#include <EGL/egl.h>
#include <glad/gl.h>
#include <stddef.h>

typedef struct {
    felidae_gl_id placeholder_texture;
} felidae_outer_state;

static felidae_batch_renderer renderer = { 0 };

felidae_payload_result
felidae_graphics_init(felidae_window_t *_, felidae_graphics_context_t *ctx)
{
    gladLoadGL((GLADloadfunc)eglGetProcAddress);
    eglSwapInterval(ctx->display, 1);
    felidae_init_batch_renderer(&renderer, 5 * 1024);
    return felidae_success();
}

void felidae_graphics_start(
    felidae_window_t *window, felidae_graphics_context_t *ctx, float delta
)
{
    struct felidae_window_dimensions dimensions
        = felidae_get_window_dimensions(window);
    float width = dimensions.width, height = dimensions.height;
    glViewport(0, 0, width, height);

    const float radius = 10.f;
    float cam_x = sin(delta) * radius, cam_y = cos(delta) * radius;

    mat4 mvp, projection, view, model = GLM_MAT4_IDENTITY_INIT;
    glm_perspective(45.f, width / height, .1f, 100.f, projection);
    glm_rotate(model, 50.0f, (float[3]) { .5f, 1.f, 0.f });
    glm_lookat(
        (vec3) { cam_x, 0.f, cam_y }, (vec3) { 0.f, 0.f, 0.f },
        (vec3) { 0.f, 1.f, 0.f }, view
    );
    glm_mat4_mulN((mat4 *[3]) { &projection, &view, &model }, 3, mvp);

    felidae_set_mvp(&renderer, mvp);
}

void felidae_graphics_cleanup(
    felidae_window_t *window, felidae_graphics_context_t *ctx
)
{
    felidae_delete_vao(renderer.vao);
    felidae_delete_buffer(renderer.vbo);
    felidae_delete_program(renderer.shader_program);
}

#define push(...) felidae_push_vertex(&renderer, __VA_ARGS__)

void DrawBackground(
    unsigned int red, unsigned int green, unsigned int blue, float alpha
)
{
    glClearColor(red / 255., green / 255., blue / 255., alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DrawSquare(void)
{
    felidae_set_texture(&renderer, renderer.uniforms.texture);
    push(0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1, 1.0f, 1.0f);
    push(0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1, 1.0f, 0.0f);
    push(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1, 0.0f, 0.0f);
    push(0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1, 1.0f, 1.0f);
    push(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1, 1.0f, 0.0f);
    push(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1, 0.0f, 0.0f);
}

void felidae_graphics_end(
    felidae_window_t *window, felidae_graphics_context_t *ctx, float delta
)
{
    DrawSquare();
    felidae_flush_vertices(&renderer);
    eglSwapBuffers(ctx->display, ctx->surface);
}
