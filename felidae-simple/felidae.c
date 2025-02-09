#include "felidae/felidae.h"
#include "bits/time.h"
#include "felidae/opengl/context.h"
#include "felidae/opengl/wrapper.h"
#include "felidae/windowing/core.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

struct felidae_basic_state {
    felidae_window_t *window;
    felidae_graphics_context_t *graphics;
    unsigned int max_fps;
    struct timespec time;
};

struct felidae_basic_state BASIC = { 0 };

#define proceed(TYPE, FNC)                                                     \
    result = felidae_##FNC;                                                    \
    if (result.kind) {                                                         \
        printf(                                                                \
            "Failed [%s]: %s [%s/%d]\n", TYPE, result.context, __FILE__,       \
            __LINE__                                                           \
        );                                                                     \
        abort();                                                               \
    }

#define precheck(X)                                                            \
    if (!BASIC.window || !BASIC.graphics)                                      \
    return X

void MakeWindow(
    int width, int height, const char *title, int preferred_display, int x,
    int y
)
{
    felidae_display_t *display;
    felidae_payload_result result;
    proceed(
        "display",
        get_preferred_display(
            &display,
            (felidae_get_preferred_display_payload) { .screen_idx
                                                      = preferred_display }
        )
    );
    proceed(
        "window",
        create_window(
            &BASIC.window, display,
            (felidae_create_window_payload) { .title = title,
                                              .width = width,
                                              .height = height,
                                              .x = x,
                                              .y = y }
        )
    );
    proceed("graphics", graphics_create_context(&BASIC.graphics, BASIC.window));
    clock_gettime(CLOCK_MONOTONIC, &BASIC.time);
}

float GetDeltaTime(void)
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (time.tv_sec - BASIC.time.tv_sec)
        + 1e-9f * (time.tv_nsec - BASIC.time.tv_nsec);
}

void SetFramerateLimit(unsigned int limit)
{
    precheck();
    BASIC.max_fps = limit;
}

struct felidae_event *PollEvent()
{
    precheck(NULL);
    struct felidae_event *event = felidae_event_poll(BASIC.window);
    return event;
}

void BeginRendering(void)
{
    precheck();
    felidae_graphics_start(BASIC.window, BASIC.graphics);
}

void FinishRendering(void)
{
    precheck();
    felidae_graphics_end(BASIC.window, BASIC.graphics);
}

bool ShouldWindowClose(void)
{
    precheck(true);
    return felidae_should_window_close(BASIC.window);
}

void RevealWindow(void)
{
    precheck();
    felidae_show_window(BASIC.window);
}

void HideWindow()
{
    precheck();
    felidae_hide_window(BASIC.window);
}

bool IsWindowHidden()
{
    precheck(true);
    return felidae_is_window_hidden(BASIC.window);
}

void FelidaeFree()
{
    if (BASIC.graphics) {
        felidae_opengl_free(BASIC.graphics);
        BASIC.graphics = NULL;
    }
    if (BASIC.window) {
        felidae_free_window(BASIC.window);
        BASIC.window = NULL;
    }
}

struct felidae_window_dimensions GetWindowDimensions()
{
    return felidae_get_window_dimensions(BASIC.window);
}

const char *GetWindowTitle()
{
    precheck(NULL);
    return felidae_get_window_title(BASIC.window);
}

felidae_window_t *GetFelidaeWindow()
{
    precheck(NULL);
    return BASIC.window;
}

felidae_graphics_context_t *GetGraphicsContext()
{
    precheck(NULL);
    return BASIC.graphics;
}
