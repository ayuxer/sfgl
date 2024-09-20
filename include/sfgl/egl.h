#ifndef SFGL_EGL_H
#define SFGL_EGL_H

#include "sfgl/payload.h"

#ifdef __unix
#include "sfgl/unix/x11/egl.h"
#endif

typedef struct sfgl_egl_display sfgl_egl_display_t;
typedef struct sfgl_egl_config sfgl_egl_config_t;
typedef struct sfgl_egl_window sfgl_egl_window_t;

enum sfgl_payload_result sfgl_egl_check_api_extensions(void);

enum sfgl_payload_result sfgl_egl_get_preferred_display(
    sfgl_egl_display_t **display, sfgl_get_preferred_display_payload payload
);

enum sfgl_payload_result sfgl_egl_init(sfgl_init_egl_payload payload);

enum sfgl_payload_result
sfgl_egl_create_config(sfgl_egl_config_t **config, sfgl_egl_display_t *display);

enum sfgl_payload_result sfgl_egl_create_window(
    sfgl_egl_window_t **window, sfgl_egl_config_t *config,
    sfgl_create_window_payload payload
);

#endif
