#ifndef FELIDAE_BASIC_H
#define FELIDAE_BASIC_H

#include "felidae/common/payload.h"
#include "felidae/opengl/graphics.h"
#include "felidae/windowing/core.h"
#include "felidae/windowing/event.h"

void MakeWindow(
    int width, int height, const char *title, int preferred_display
);

struct felidae_event *PollEvent();

bool ShouldWindowClose();

void RevealWindow();

void HideWindow();

bool IsWindowHidden();

void FelidaeFree();

int GetWindowWidth();

int GetWindowHeight();

int GetWindowX();

int GetWindowY();

const char *GetWindowTitle();

#endif
