#include "felidae/windowing/event.h"
#include "felidae/windowing/core.h"
#include <stdlib.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

static bool init_felidae_event(
    struct felidae_event *input, xcb_generic_event_t *event, int type
)
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

struct felidae_event *felidae_event_poll(felidae_window_t *window)
{
    if (!window)
        return NULL;
    xcb_generic_event_t *event = xcb_poll_for_event(window->display->x11);
    if (!event)
        return NULL;
    const unsigned int type = event ? event->response_type & ~0x80 : 0;
    struct felidae_event *out = malloc(sizeof(struct felidae_event));
    if (!out) {
        free(event);
        return NULL;
    }
    switch (type) {
        case XCB_RESIZE_REQUEST: {
            auto ev = (xcb_resize_request_event_t *)event;
            out->kind = RESIZE;
            out->data.resize.new_width = ev->width;
            out->data.resize.new_height = ev->height;
            break;
        }
        case XCB_CLIENT_MESSAGE: {
            auto ev = (xcb_client_message_event_t *)event;
            if (ev->data.data32[0] == window->close_atom->atom) {
                window->should_close = true;
                return NULL;
            }
            break;
        }
        default: {
            if (!init_felidae_event(out, event, type)) {
                free(event);
                free(out);
                return NULL;
            }
            break;
        }
    }
    free(event);
    return out;
}
