#ifndef FELIDAE_GRAPHICS_H
#define FELIDAE_GRAPHICS_H

#include "felidae/common/payload.h"
#include "felidae/windowing/core.h"

#ifdef FELIDAE_USE_OPENGL
#include "felidae/opengl/context.h"
#include "felidae/opengl/payload.h"
#endif

typedef struct felidae_graphics_context felidae_graphics_context_t;

enum felidae_payload_result felidae_check_api_extensions(void);

enum felidae_payload_result felidae_graphics_create_context(
    felidae_graphics_context_t **ctx, felidae_window_t *window
);

#endif
