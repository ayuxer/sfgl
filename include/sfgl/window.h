#ifndef SFGL_WINDOW_H
#define SFGL_WINDOW_H

#include "sfgl/egl.h"
#include "sfgl/payload.h"

typedef sfgl_egl_window_t sfgl_window_t;

void sfgl_window_show(sfgl_window_t *window);

void sfgl_window_hide(sfgl_window_t *window);

void sfgl_window_update(sfgl_window_t *win, sfgl_update_window_payload payload);

char *sfgl_window_get_title(sfgl_window_t *window);

int sfgl_window_get_width(sfgl_window_t *window);

int sfgl_window_get_height(sfgl_window_t *window);

int sfgl_window_get_x(sfgl_window_t *window);

int sfgl_window_get_y(sfgl_window_t *window);

_Bool sfgl_window_was_closed(sfgl_window_t *window);

void sfgl_free(sfgl_window_t *window);

#endif
