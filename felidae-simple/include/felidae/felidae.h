#ifndef FELIDAE_BASIC_H
#define FELIDAE_BASIC_H

#include "felidae/common/payload.h"
#include "felidae/opengl/context.h"
#include "felidae/opengl/graphics.h"
#include "felidae/opengl/payload.h"
#include "felidae/windowing/core.h"
#include "felidae/windowing/event.h"

void MakeWindow(
    int width, int height, const char *title, int preferred_display, int x,
    int y
);

struct felidae_event *PollEvent(void);

void SetFramerateLimit(unsigned int limit);

void BeginRendering(void);

void FinishRendering(void);

bool ShouldWindowClose(void);

void RevealWindow(void);

void HideWindow(void);

bool IsWindowHidden(void);

void FelidaeFree(void);

float GetDeltaTime(void);

struct felidae_window_dimensions GetWindowDimensions(void);

const char *GetWindowTitle(void);

felidae_window_t *GetFelidaeWindow(void);

felidae_graphics_context_t *GetGraphicsContext(void);

#endif
