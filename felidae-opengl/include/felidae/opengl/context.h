#ifndef FELIDAE_OPENGL_CONTEXT_H
#define FELIDAE_OPENGL_CONTEXT_H

#include "felidae/common/payload.h"
#include "felidae/opengl/payload.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

#define CORE EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT
#define COMPATIBILITY EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT

constexpr int EGL_MIN_VER_MINOR = 1;
constexpr int EGL_MIN_VER_MAJOR = 5;
constexpr int OPENGL_MIN_VER_MAJOR = 4;
constexpr int OPENGL_MIN_VER_MINOR = 6;
constexpr int OPENGL_PROFILE = CORE;

struct felidae_graphics_context {
    EGLContext egl;
    EGLSurface surface;
    EGLConfig config;
    EGLDisplay display;
};

felidae_payload_result felidae_opengl_init(felidae_init_opengl_payload payload);

void felidae_opengl_free(struct felidae_graphics_context *ctx);

#endif
