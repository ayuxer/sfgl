#ifndef SFGL_OPENGL_CONTEXT_H
#define SFGL_OPENGL_CONTEXT_H

#include "sfgl/payload.h"
#include <EGL/egl.h>

struct sfgl_graphics_context {
    EGLContext egl;
    EGLSurface surface;
    EGLConfig config;
    EGLDisplay display;
};

enum sfgl_payload_result sfgl_opengl_init(sfgl_init_opengl_payload payload);

#endif
