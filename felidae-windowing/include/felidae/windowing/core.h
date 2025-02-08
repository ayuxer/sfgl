#ifndef FELIDAE_WINDOWING_H
#define FELIDAE_WINDOWING_H

#include "felidae/common/payload.h"

#ifdef __unix__
#define __felidae_x11__
#endif

#ifdef __felidae_x11__
#include "felidae/windowing/unix/x11/core.h"
#endif

typedef struct felidae_display felidae_display_t;
typedef struct felidae_window felidae_window_t;

struct felidae_window_dimensions {
    int width, height;
};

felidae_payload_result felidae_get_preferred_display(
    felidae_display_t **display, felidae_get_preferred_display_payload payload
);

felidae_payload_result felidae_create_window(
    felidae_window_t **window, felidae_display_t *display,
    felidae_create_window_payload payload
);

void felidae_show_window(felidae_window_t *window);

void felidae_hide_window(felidae_window_t *window);

bool felidae_is_window_hidden(felidae_window_t *window);

void felidae_modify_window(
    felidae_window_t *win, felidae_update_window_payload payload
);

const char *felidae_get_window_title(felidae_window_t *window);

struct felidae_window_dimensions
felidae_get_window_dimensions(felidae_window_t *window);

bool felidae_should_window_close(felidae_window_t *window);

void felidae_free_window(felidae_window_t *window);

unsigned int felidae_get_window_id(felidae_display_t *display);

#endif
