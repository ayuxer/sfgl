#ifndef SFGL_BASIC_H
#define SFGL_BASIC_H

#include "sfgl/event.h"

void MakeWindow(
    int width, int height, const char *title, int preferred_display
);

struct sfgl_event *PollEvent();

bool ShouldWindowClose();

void RevealWindow();

void HideWindow();

bool IsWindowHidden();

void SFGLFree();

int GetWindowWidth();

int GetWindowHeight();

int GetWindowX();

int GetWindowY();

const char *GetWindowTitle();

#endif
