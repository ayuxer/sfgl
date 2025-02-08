#ifndef FELIDAE_EGL_X11_H
#define FELIDAE_EGL_X11_H

#include <felidae/common/payload.h>
#include <xcb/xcb.h>

struct felidae_display {
    xcb_connection_t *x11;
    xcb_screen_t *screen;
};

struct felidae_window {
    struct felidae_display *display;
    xcb_window_t x11;
    xcb_intern_atom_reply_t *close_atom;
    bool should_close;
};

felidae_payload_result felidae_x11_init_window_colormap(
    struct felidae_window *window, unsigned int visualid
);

#endif
