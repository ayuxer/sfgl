#ifndef FELIDAE_PAYLOAD_OPENGL_H
#define FELIDAE_PAYLOAD_OPENGL_H

#include <EGL/egl.h>

typedef struct {
    int min_major_egl_version, min_minor_egl_version;
    EGLDisplay egl;
} felidae_init_opengl_payload;

#endif
