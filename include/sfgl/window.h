#ifndef SFGL_WINDOW_H
#define SFGL_WINDOW_H

#include "sfgl/payload.h"

#ifdef __unix
#define __sfgl_x11__
#include "sfgl/unix/x11/window.h"
#endif

typedef struct sfgl_display sfgl_display_t;
typedef struct sfgl_window sfgl_window_t;

enum sfgl_payload_result sfgl_get_preferred_display(
    sfgl_display_t **display, sfgl_get_preferred_display_payload payload
);

enum sfgl_payload_result sfgl_create_window(
    sfgl_window_t **window, sfgl_display_t *display,
    sfgl_create_window_payload payload
);

void sfgl_show_window(sfgl_window_t *window);

void sfgl_hide_window(sfgl_window_t *window);

void sfgl_modify_window(sfgl_window_t *win, sfgl_update_window_payload payload);

char *sfgl_get_window_title(sfgl_window_t *window);

int sfgl_get_window_width(sfgl_window_t *window);

int sfgl_get_window_height(sfgl_window_t *window);

int sfgl_get_window_x(sfgl_window_t *window);

int sfgl_get_window_y(sfgl_window_t *window);

_Bool sfgl_is_window_requested_for_closure(sfgl_window_t *window);

void sfgl_free_window(sfgl_window_t *window);

#endif
