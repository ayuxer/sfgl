#include "felidae/opengl/graphics.h"
#include "felidae/windowing/core.h"

#include <EGL/egl.h>
#include <glad/gl.h>
#include <stddef.h>

void felidae_graphics_init(felidae_window_t *_, felidae_graphics_context_t *ctx)
{
    gladLoadGL((GLADloadfunc)eglGetProcAddress);
    eglSwapInterval(ctx->display, 1);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    eglSwapBuffers(ctx->display, ctx->surface);
}

void DrawBackground(
    unsigned int red, unsigned int green, unsigned int blue, float alpha
)
{
    glClearColor(red / 255., green / 255., blue / 255., alpha);
}
