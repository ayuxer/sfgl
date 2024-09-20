#include "sfgl/egl.h"
#include "sfgl/event.h"
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
    if (window->was_close_requested) {
        printf("Close requested!\n");
        fflush(stdout);
        exit(0);
    }
    if (!event || event->kind != KEY_PRESS)
        return;
    auto key_press = (sfgl_key_press_event *)event->data;
    const int width = sfgl_window_get_width(window),
              height = sfgl_window_get_height(window);
    const char *title = sfgl_window_get_title(window);
    printf(
        "Key press: code=%d, width=%d, height=%d, title=%s\n",
        key_press->key_code, width, height, title
    );
    fflush(stdout);
}

int main(void)
{
    sfgl_window_t *win;
    sfgl_egl_display_t *display;
    sfgl_egl_config_t *config;
    validate(
        sfgl_egl_get_preferred_display(
            &display, (sfgl_get_preferred_display_payload) { 0 }
        ),
        "display"
    );
    validate(
        sfgl_egl_init((sfgl_init_egl_payload) { .egl = display->egl }), "egl"
    );
    validate(sfgl_egl_create_config(&config, display), "config");
    validate(
        sfgl_egl_create_window(
            &win, config,
            (sfgl_create_window_payload) { .title = "Hello, world!",
                                           .width = 640,
                                           .height = 480,
                                           .x = 0,
                                           .y = 0 }
        ),
        "window"
    );
    sfgl_window_show(win);
    sfgl_event_start_main_loop(win, handle_notification);
    sfgl_free(win);
    return 0;
}
