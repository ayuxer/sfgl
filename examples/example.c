#include "sfgl/event.h"
#include "sfgl/graphics.h"
#include "sfgl/payload.h"
#include "sfgl/window.h"
#include <stdio.h>
#include <stdlib.h>

static void validate(enum sfgl_payload_result result, char *type)
{
    if (result) {
        printf("Failed [%s]: %d\n", type, result);
        exit(1);
    }
}

static void handle_notification(sfgl_window_t *window, sfgl_event *event)
{
    if (window->is_closure_requested) {
        printf("Close requested!\n");
        fflush(stdout);
        exit(0);
    }
    if (!event || event->kind != KEY_PRESS)
        return;
    auto key_press = (sfgl_key_press_event *)event->data;
    const int width = sfgl_get_window_width(window),
              height = sfgl_get_window_height(window);
    const char *title = sfgl_get_window_title(window);
    printf(
        "-> Key press: code=%d, width=%d, height=%d, title=%s\n",
        key_press->key_code, width, height, title
    );
    fflush(stdout);
}

int main(void)
{
    sfgl_window_t *window;
    sfgl_display_t *display;
    sfgl_graphics_context_t *graphics;
    validate(
        sfgl_get_preferred_display(
            &display, (sfgl_get_preferred_display_payload) { 0 }
        ),
        "display"
    );
    validate(
        sfgl_create_window(
            &window, display,
            (sfgl_create_window_payload) { .title = "Hello, world!",
                                           .width = 640,
                                           .height = 480,
                                           .x = 0,
                                           .y = 0 }
        ),
        "window"
    );
    validate(sfgl_graphics_create_context(&graphics, window), "egl");
    sfgl_show_window(window);
    sfgl_event_start_main_loop(window, handle_notification);
    sfgl_free_window(window);
    return 0;
}
