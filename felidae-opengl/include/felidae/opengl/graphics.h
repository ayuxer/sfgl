#ifndef FELIDAE_GRAPHICS_H
#define FELIDAE_GRAPHICS_H

#include "felidae/common/payload.h"
#include "felidae/windowing/core.h"

#include "felidae/opengl/context.h"

typedef struct felidae_graphics_context felidae_graphics_context_t;

felidae_payload_result felidae_check_api_extensions(void);

felidae_payload_result felidae_graphics_create_context(
    felidae_graphics_context_t **ctx, felidae_window_t *window
);

void felidae_graphics_start(
    felidae_window_t *window, felidae_graphics_context_t *ctx
);

void felidae_graphics_end(
    felidae_window_t *window, felidae_graphics_context_t *ctx
);

void felidae_graphics_init(
    felidae_window_t *window, felidae_graphics_context_t *ctx
);

// Wrappers

void DrawBackground(
    unsigned int red, unsigned int green, unsigned int blue, float alpha
);

#endif
