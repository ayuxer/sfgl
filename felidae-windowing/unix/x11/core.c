#include "felidae/windowing/core.h"
#include "felidae/common/payload.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

static xcb_screen_t *get_screen(xcb_connection_t *c, int screen)
{
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(c));
    for (; iter.rem; --screen, xcb_screen_next(&iter))
        if (screen == 0)
            return iter.data;
    return NULL;
}

unsigned int felidae_get_window_id(felidae_display_t *display)
{
    xcb_screen_iterator_t iter
        = xcb_setup_roots_iterator(xcb_get_setup(display->x11));
    for (int id = 0; iter.rem; xcb_screen_next(&iter), id++)
        if (iter.data == display->screen)
            return id;
    return 0;
}

felidae_payload_result felidae_get_preferred_display(
    felidae_display_t **display, felidae_get_preferred_display_payload payload
)
{
    *display = malloc(sizeof(felidae_display_t));
    if (((*display)->x11 = xcb_connect(NULL, &payload.screen_idx)) == NULL) {
        free(*display);
        return felidae_decontextualized(FAILED_TO_CONNECT_TO_DISPLAY_SERVER);
    }
    if (xcb_connection_has_error((*display)->x11)) {
        free(*display);
        return felidae_decontextualized(FAILED_TO_CONNECT_TO_DISPLAY_SERVER);
    }
    (*display)->screen = get_screen((*display)->x11, payload.screen_idx);
    if (!(*display)->screen) {
        xcb_disconnect((*display)->x11);
        free(*display);
        return felidae_decontextualized(CANT_OPEN_DISPLAY);
    }
    return felidae_success();
}

felidae_payload_result felidae_create_window(
    felidae_window_t **window, felidae_display_t *display,
    felidae_create_window_payload payload
)
{
    *window = malloc(sizeof(felidae_window_t));
    (*window)->display = display;
    (*window)->x11 = xcb_generate_id(display->x11);

    const unsigned int value_list[2]
        = { display->screen->black_pixel,
            XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS
                | XCB_EVENT_MASK_KEY_RELEASE };
    const unsigned int value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    xcb_generic_error_t *err = xcb_request_check(
        display->x11,
        xcb_create_window_checked(
            display->x11, XCB_COPY_FROM_PARENT, (*window)->x11,
            display->screen->root, payload.x, payload.y, payload.width,
            payload.height, 1, XCB_WINDOW_CLASS_INPUT_OUTPUT,
            display->screen->root_visual, value_mask, value_list
        )
    );
    if (err) {
        free(err);
        felidae_free_window(*window);
        return felidae_decontextualized(FAILED_TO_CREATE_WINDOW);
    }

    if (payload.title)
        xcb_change_property(
            display->x11, XCB_PROP_MODE_REPLACE, (*window)->x11,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(payload.title),
            payload.title
        );

    xcb_intern_atom_cookie_t proto_cookie
        = xcb_intern_atom(display->x11, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *proto_reply
        = xcb_intern_atom_reply(display->x11, proto_cookie, 0);
    xcb_intern_atom_cookie_t close_cookie
        = xcb_intern_atom(display->x11, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *close_reply
        = xcb_intern_atom_reply(display->x11, close_cookie, 0);
    xcb_change_property(
        display->x11, XCB_PROP_MODE_REPLACE, (*window)->x11, proto_reply->atom,
        4, 32, 1, &close_reply->atom
    );
    (*window)->close_atom = close_reply;
    free(proto_reply);

    (*window)->should_close = false;
    return felidae_success();
}

void felidae_show_window(felidae_window_t *window)
{
    if (!window || !window->display || !window->x11)
        return;
    xcb_map_window(window->display->x11, window->x11);
    xcb_flush(window->display->x11);
}

void felidae_hide_window(felidae_window_t *window)
{
    if (!window || !window->display || !window->x11)
        return;
    xcb_unmap_window(window->display->x11, window->x11);
    xcb_flush(window->display->x11);
}

bool felidae_is_window_hidden(felidae_window_t *window)
{
    if (!window || !window->display || !window->x11)
        return true;

    xcb_get_window_attributes_cookie_t cookie
        = xcb_get_window_attributes(window->display->x11, window->x11);
    xcb_get_window_attributes_reply_t *reply
        = xcb_get_window_attributes_reply(window->display->x11, cookie, NULL);
    if (!reply)
        return true;
    bool is_hidden = reply->map_state != XCB_MAP_STATE_VIEWABLE;
    free(reply);
    return is_hidden;
}

void felidae_modify_window(
    felidae_window_t *window, felidae_update_window_payload payload
)
{
    if (felidae_should_window_close(window))
        return;
    uint16_t mask = 0;
    xcb_configure_window_value_list_t values = { 0 };
    if (payload.width) {
        mask |= XCB_CONFIG_WINDOW_WIDTH;
        values.width = *payload.width;
    }
    if (payload.height) {
        mask |= XCB_CONFIG_WINDOW_HEIGHT;
        values.width = *payload.height;
    }
    xcb_configure_window_aux(window->display->x11, window->x11, mask, &values);
    if (payload.title)
        xcb_change_property(
            window->display->x11, XCB_PROP_MODE_REPLACE, window->x11,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(payload.title),
            payload.title
        );
    xcb_flush(window->display->x11);
}

const char *felidae_get_window_title(felidae_window_t *window)
{
    if (felidae_should_window_close(window))
        return NULL;
    const xcb_get_property_cookie_t cookie = xcb_get_property(
        window->display->x11, 0, window->x11, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        0, 0
    );
    xcb_get_property_reply_t *reply
        = xcb_get_property_reply(window->display->x11, cookie, NULL);
    if (!reply)
        return NULL;
    int length = xcb_get_property_value_length(reply);
    if (length == 0) {
        free(reply);
        return NULL;
    }
    char *value = xcb_get_property_value(reply);
    free(reply);
    return value;
}

struct felidae_window_dimensions
felidae_get_window_dimensions(felidae_window_t *window)
{
    if (felidae_should_window_close(window))
        return (struct felidae_window_dimensions) { 0 };
    xcb_get_geometry_cookie_t cookie
        = xcb_get_geometry(window->display->x11, window->x11);
    xcb_get_geometry_reply_t *reply
        = xcb_get_geometry_reply(window->display->x11, cookie, NULL);
    struct felidae_window_dimensions value = (struct felidae_window_dimensions
    ) { .width = reply->width, .height = reply->height };
    free(reply);
    return value;
}

bool felidae_should_window_close(felidae_window_t *window)
{
    return !window || window->should_close;
}

void felidae_free_window(felidae_window_t *window)
{
    if (!window)
        return;
    if (window->display)
        xcb_disconnect(window->display->x11);
    free(window);
}
