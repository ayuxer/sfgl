#ifndef SFGL_GRAPHICS_H
#define SFGL_GRAPHICS_H

#include "sfgl/payload.h"
#include "sfgl/window.h"
#include "sfgl/opengl/context.h"

#define __sfgl_opengl__

typedef struct sfgl_graphics_context sfgl_graphics_context_t;

enum sfgl_payload_result sfgl_check_api_extensions(void);

enum sfgl_payload_result sfgl_graphics_create_context(sfgl_graphics_context_t **ctx, sfgl_window_t* window);

#endif
