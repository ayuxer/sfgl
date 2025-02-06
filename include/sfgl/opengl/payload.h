#ifndef SFGL_PAYLOAD_OPENGL_H
#define SFGL_PAYLOAD_OPENGL_H

#include <EGL/egl.h>

typedef struct {
    int min_major_egl_version, min_minor_egl_version;
    EGLDisplay egl;
} sfgl_init_opengl_payload;

#endif
