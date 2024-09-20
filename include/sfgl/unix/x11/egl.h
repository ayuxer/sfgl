#ifndef SFGL_EGL_X11_H
#define SFGL_EGL_X11_H

#include <EGL/egl.h>
#include <xcb/xcb.h>

struct sfgl_egl_display {
    xcb_connection_t *x11;
    int screen_idx;
    int root_of_screen_idx;
    EGLDisplay egl;
};

struct sfgl_egl_config {
    struct sfgl_egl_display *display;
    EGLContext ctx;
    EGLConfig egl;
};

struct sfgl_egl_window {
    struct sfgl_egl_config *config;
    xcb_colormap_t colormap;
    xcb_window_t x11;
    EGLSurface egl;
    int width, height, x, y;
    xcb_intern_atom_reply_t* close_atom;
    _Bool was_close_requested;
};

#endif
