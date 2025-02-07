#include "felidae/felidae.h"
#include <stdio.h>
#include <stdlib.h>

struct felidae_basic_state {
    felidae_window_t *window;
    felidae_graphics_context_t *graphics;
    bool failed;
};

struct felidae_basic_state BASIC = { 0 };

static const char *const FELIDAE_PAYLOAD_MESSAGES[] = {
    [SUCCESS] = "Operation completed successfully",
    [MISSING_API_EXTENSIONS] = "Required API extensions are not available",
    [FAILED_TO_CONNECT_TO_DISPLAY_SERVER]
    = "Could not establish connection to display server",
    [FAILED_TO_INITIALIZE_EGL] = "Failed to initialize EGL",
    [FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG]
    = "No compatible EGL configuration found",
    [FAILED_TO_GET_VISUAL_ID] = "Could not obtain visual ID",
    [FAILED_TO_INITIALIZE_COLORMAP] = "Failed to initialize colormap",
    [FAILED_TO_CREATE_WINDOW] = "Window creation failed",
    [FAILED_TO_CHANGE_WINDOW_ATTRIBUTES] = "Could not change window attributes",
    [FAILED_TO_CREATE_WINDOW_SURFACE] = "Failed to create window surface",
    [OUTDATED_EGL] = "EGL version is outdated",
    [FAILED_TO_BIND_OPENGL] = "Failed to bind OpenGL context",
    [CANT_OPEN_DISPLAY] = "Could not open display",
};

const char *felidae_get_payload_result_message(enum felidae_payload_result input
)
{
    if (input >= 0 && input <= CANT_OPEN_DISPLAY) {
        return FELIDAE_PAYLOAD_MESSAGES[input];
    }
    return "Unknown error";
}

#define proceed(TYPE)                                                          \
    if (result) {                                                              \
        printf(                                                                \
            "Failed [%s]: %s\n", TYPE,                                         \
            felidae_get_payload_result_message(result)                         \
        );                                                                     \
        BASIC.failed = true;                                                   \
    }                                                                          \
    if (BASIC.failed)

#define precheck(X)                                                            \
    if (BASIC.failed || !BASIC.window)                                         \
    return X

void MakeWindow(int width, int height, const char *title, int preferred_display)
{
    felidae_window_t *window;
    felidae_display_t *display;
    felidae_graphics_context_t *graphics;
    enum felidae_payload_result result = felidae_get_preferred_display(
        &display,
        (felidae_get_preferred_display_payload) { .screen_idx
                                                  = preferred_display }
    );
    proceed("display") { return; }
    result = felidae_create_window(
        &window, display,
        (felidae_create_window_payload
        ) { .title = title, .width = width, .height = height, .x = 0, .y = 0 }
    );
    proceed("window")
    {
        free(display);
        return;
    }
    result = felidae_graphics_create_context(&graphics, window);
    proceed("graphics")
    {
        free(window);
        free(graphics);
        return;
    }
    BASIC.graphics = graphics;
    BASIC.window = window;
}

struct felidae_event *PollEvent()
{
    precheck(NULL);
    return felidae_event_poll(BASIC.window);
}

bool ShouldWindowClose()
{
    precheck(true);
    return felidae_should_window_close(BASIC.window);
}

void RevealWindow()
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
    precheck();
    felidae_free_window(BASIC.window);
    free(BASIC.graphics);
}

int GetWindowWidth()
{
    precheck(-1);
    return felidae_get_window_width(BASIC.window);
}

int GetWindowHeight()
{
    precheck(-1);
    return felidae_get_window_height(BASIC.window);
}

int GetWindowX()
{
    precheck(-1);
    return felidae_get_window_x(BASIC.window);
}

int GetWindowY()
{
    precheck(-1);
    return felidae_get_window_y(BASIC.window);
}

const char *GetWindowTitle()
{
    precheck(NULL);
    return felidae_get_window_title(BASIC.window);
}
