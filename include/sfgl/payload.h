#ifndef SFGL_PAYLOAD_H
#define SFGL_PAYLOAD_H

#include <EGL/egl.h>

enum sfgl_payload_result {
    SUCCESS = 0,
    MISSING_API_EXTENSIONS = 1,
    FAILED_TO_CONNECT_TO_DISPLAY_SERVER = 2,
    FAILED_TO_INITIALIZE_EGL = 3,
    FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG = 4,
    FAILED_TO_GET_VISUAL_ID = 5,
    FAILED_TO_INITIALIZE_COLORMAP = 6,
    FAILED_TO_CREATE_WINDOW = 7,
    FAILED_TO_CHANGE_WINDOW_ATTRIBUTES = 8,
    FAILED_TO_CREATE_WINDOW_SURFACE = 9,
    OUTDATED_EGL = 10,
    FAILED_TO_BIND_OPENGL = 11,
    CANT_OPEN_DISPLAY = 12
};

typedef struct {
    int screen_idx;
} sfgl_get_preferred_display_payload;

typedef struct {
    char *title;
    int width, height;
    int x, y;
} sfgl_create_window_payload;

typedef struct {
    int min_major_egl_version, min_minor_egl_version;
    EGLDisplay egl;
} sfgl_init_opengl_payload;

typedef struct {
    char *title;
    unsigned int *width;
    unsigned int *height;
    unsigned int *x;
    unsigned int *y;
} sfgl_update_window_payload;

#endif
