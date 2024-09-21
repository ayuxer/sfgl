#ifndef SFGL_EGL_X11_H
#define SFGL_EGL_X11_H

#include <xcb/xcb.h>

struct sfgl_display {
    xcb_connection_t *x11;
    int screen_idx;
    int root_of_screen_idx;
};

struct sfgl_window {
    struct sfgl_display *display;
    xcb_colormap_t colormap;
    xcb_window_t x11;
    int width, height, x, y;
    xcb_intern_atom_reply_t* close_atom;
    _Bool is_closure_requested;
};

#endif
