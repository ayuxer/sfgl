#ifndef FELIDAE_OPENGL_CONTEXT_H
#define FELIDAE_OPENGL_CONTEXT_H

#include "felidae/common/payload.h"
#include "felidae/opengl/payload.h"
#include <EGL/egl.h>

struct felidae_graphics_context {
    EGLContext egl;
    EGLSurface surface;
    EGLConfig config;
    EGLDisplay display;
};

felidae_payload_result felidae_opengl_init(felidae_init_opengl_payload payload);

void felidae_opengl_free(struct felidae_graphics_context *ctx);

#endif
