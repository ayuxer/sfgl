#include "sfgl/event.h"
#include "sfgl/window.h"
#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#define notify(XCB, KIND, DATA)                                                \
    {                                                                          \
        XCB *ev = (XCB *)event;                                                \
        notify_(                                                               \
            window,                                                            \
            &((sfgl_event) { .kind = (KIND),                                   \
                             .timestamp = ev->time,                            \
                             .data = (void *)&(DATA) })                        \
        );                                                                     \
    }                                                                          \
    break

static void handle_event(
    xcb_generic_event_t *event, sfgl_window_t *window, int type,
    main_loop_callback notify_
)
{
    if (!event) {
        notify_(window, NULL);
        return;
    }
    switch (type) {
    case XCB_KEY_PRESS:
        notify(
            xcb_key_press_event_t, KEY_PRESS,
            (sfgl_key_press_event) { .key_code = ev->detail }
        );
    case XCB_KEY_RELEASE:
        notify(
            xcb_key_release_event_t, KEY_RELEASE,
            (sfgl_key_release_event) { .key_code = ev->detail }
        );
    default:
        break;
    }
}

void sfgl_event_start_main_loop(sfgl_window_t *window, main_loop_callback loop)
{
    if (sfgl_is_window_requested_for_closure(window))
        return;
    xcb_generic_event_t *event;

    while (true) {
        event = xcb_poll_for_event(window->display->x11);
        const unsigned int type = event ? event->response_type & ~0x80 : 0;
        if (type == XCB_RESIZE_REQUEST) {
            auto ev = (xcb_resize_request_event_t *)event;
            window->width = ev->width;
            window->height = ev->height;
        } else if (type == XCB_CLIENT_MESSAGE) {
            auto ev = (xcb_client_message_event_t *)event;
            if (ev->data.data32[0] == window->close_atom->atom)
                window->is_closure_requested = true;
        }
        handle_event(event, window, type, loop);
    }
    if (event)
        free(event);
}
