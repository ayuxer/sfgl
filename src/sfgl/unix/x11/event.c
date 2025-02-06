#include "sfgl/event.h"
#include "sfgl/window.h"
#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

static bool
init_sfgl_event(struct sfgl_event *input, xcb_generic_event_t *event, int type)
{
    switch (type) {
    case XCB_KEY_PRESS: {
        xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
        input->kind = KEY_PRESS;
        input->timestamp = ev->time;
        input->data.key_press.key_code = ev->detail;
        return true;
    }
    case XCB_KEY_RELEASE: {
        xcb_key_release_event_t *ev = (xcb_key_release_event_t *)event;
        input->kind = KEY_RELEASE;
        input->timestamp = ev->time;
        input->data.key_release.key_code = ev->detail;
        return true;
    }
    default:
        return false;
    }
}

struct sfgl_event *sfgl_event_poll(sfgl_window_t *window)
{
    if (!window)
        return NULL;
    xcb_generic_event_t *event = xcb_poll_for_event(window->display->x11);
    if (!event)
        return NULL;
    const unsigned int type = event ? event->response_type & ~0x80 : 0;
    if (type == XCB_RESIZE_REQUEST) {
        auto ev = (xcb_resize_request_event_t *)event;
        window->width = ev->width;
        window->height = ev->height;
    } else if (type == XCB_CLIENT_MESSAGE) {
        auto ev = (xcb_client_message_event_t *)event;
        if (ev->data.data32[0] == window->close_atom->atom)
            window->should_close = true;
    }
    struct sfgl_event *out = malloc(sizeof(struct sfgl_event));
    if (!out) {
        free(event);
        return NULL;
    }
    if (!init_sfgl_event(out, event, type)) {
        free(out);
        free(event);
        return NULL;
    }
    free(event);
    return out;
}
