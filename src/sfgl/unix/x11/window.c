#include "sfgl/window.h"
#include "sfgl/payload.h"
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

enum sfgl_payload_result sfgl_get_preferred_display(
    sfgl_display_t **display, sfgl_get_preferred_display_payload payload
)
{
    *display = malloc(sizeof(sfgl_display_t));
    if (((*display)->x11 = xcb_connect(NULL, &payload.screen_idx)) == NULL) {
        fprintf(stderr, "Failed to connect to X11 display server.\n");
        free(*display);
        return FAILED_TO_CONNECT_TO_DISPLAY_SERVER;
    }
    (*display)->screen_idx = payload.screen_idx;
    (*display)->root_of_screen_idx
        = get_screen((*display)->x11, (*display)->screen_idx)->root;
    return SUCCESS;
}

enum sfgl_payload_result
sfgl_x11_init_window_colormap(sfgl_window_t *window, unsigned int visualid)
{
    window->colormap = xcb_generate_id(window->display->x11);

    xcb_generic_error_t *err = xcb_request_check(
        window->display->x11,
        xcb_create_colormap_checked(
            window->display->x11, XCB_COLORMAP_ALLOC_NONE, window->colormap,
            window->display->root_of_screen_idx, visualid
        )
    );
    if (err) {
        fprintf(stderr, "Failed to initialize X11 colormap.\n");
        free(window);
        free(err);
        return FAILED_TO_INITIALIZE_COLORMAP;
    }

    err = xcb_request_check(
        window->display->x11,
        xcb_change_window_attributes_checked(
            window->display->x11, window->x11, XCB_CW_COLORMAP,
            (const unsigned int[]) { window->colormap }
        )
    );
    if (err) {
        fprintf(stderr, "Failed to override X11 window attributes.\n");
        free(window);
        free(err);
        return FAILED_TO_CHANGE_WINDOW_ATTRIBUTES;
    }

    return SUCCESS;
}

enum sfgl_payload_result sfgl_create_window(
    sfgl_window_t **window, sfgl_display_t *display,
    sfgl_create_window_payload payload
)
{
    *window = malloc(sizeof(sfgl_window_t));
    (*window)->display = display;
    (*window)->x11 = xcb_generate_id(display->x11);
    xcb_screen_t *screen = get_screen(display->x11, display->screen_idx);

    // clang-format off
    const unsigned int value_list[2]
        = { screen->black_pixel,
            XCB_EVENT_MASK_EXPOSURE
                | XCB_EVENT_MASK_STRUCTURE_NOTIFY
                | XCB_EVENT_MASK_BUTTON_PRESS
                | XCB_EVENT_MASK_BUTTON_RELEASE
                | XCB_EVENT_MASK_KEY_PRESS
                | XCB_EVENT_MASK_KEY_RELEASE
                | XCB_EVENT_MASK_RESIZE_REDIRECT };
    const unsigned int value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    // clang-format on

    xcb_generic_error_t *err = xcb_request_check(
        display->x11,
        xcb_create_window_checked(
            display->x11, XCB_COPY_FROM_PARENT, (*window)->x11,
            display->root_of_screen_idx, payload.x, payload.y, payload.width,
            payload.height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
            XCB_COPY_FROM_PARENT, value_mask, value_list
        )
    );
    if (err) {
        fprintf(stderr, "Failed to create X11 window\n.");
        free(err);
        free(*window);
        return FAILED_TO_CREATE_WINDOW;
    }

    (*window)->x = payload.x;
    (*window)->y = payload.y;
    (*window)->width = payload.width;
    (*window)->height = payload.height;

    if (payload.title) {
        xcb_change_property(
            display->x11, XCB_PROP_MODE_REPLACE, (*window)->x11,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(payload.title),
            payload.title
        );
    }

    auto proto_cookie = xcb_intern_atom(display->x11, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *proto_reply
        = xcb_intern_atom_reply(display->x11, proto_cookie, 0);
    auto close_cookie
        = xcb_intern_atom(display->x11, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *close_reply
        = xcb_intern_atom_reply(display->x11, close_cookie, 0);
    xcb_change_property(
        display->x11, XCB_PROP_MODE_REPLACE, (*window)->x11, proto_reply->atom,
        4, 32, 1, &close_reply->atom
    );
    (*window)->close_atom = close_reply;

    return SUCCESS;
}

void sfgl_show_window(sfgl_window_t *window)
{
    if (!window || !window->display || !window->x11)
        return;
    xcb_map_window(window->display->x11, window->x11);
    xcb_flush(window->display->x11);
}

void sfgl_hide_window(sfgl_window_t *window)
{
    if (!window || !window->display || !window->x11)
        return;
    xcb_unmap_window(window->display->x11, window->x11);
    xcb_flush(window->display->x11);
}

bool sfgl_is_window_hidden(sfgl_window_t *window)
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

#define update(MASK, FIELD)                                                    \
    {                                                                          \
        mask |= (MASK);                                                        \
        values.FIELD = *payload.FIELD;                                         \
        window->FIELD = *payload.FIELD;                                        \
    }

void sfgl_modify_window(
    sfgl_window_t *window, sfgl_update_window_payload payload
)
{
    if (sfgl_should_window_close(window))
        return;
    uint16_t mask = 0;
    xcb_configure_window_value_list_t values = { 0 };
    if (payload.x)
        update(XCB_CONFIG_WINDOW_X, x);
    if (payload.y)
        update(XCB_CONFIG_WINDOW_Y, y);
    if (payload.width)
        update(XCB_CONFIG_WINDOW_WIDTH, width);
    if (payload.height)
        update(XCB_CONFIG_WINDOW_HEIGHT, height);
    xcb_configure_window_aux(window->display->x11, window->x11, mask, &values);
    if (payload.title) {
        xcb_change_property(
            window->display->x11, XCB_PROP_MODE_REPLACE, window->x11,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(payload.title),
            payload.title
        );
    }
    xcb_flush(window->display->x11);
}

const char *sfgl_get_window_title(sfgl_window_t *window)
{
    if (sfgl_should_window_close(window))
        return NULL;
    const xcb_get_property_cookie_t cookie = xcb_get_property(
        window->display->x11, 0, window->x11, XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
        0, 0
    );
    xcb_get_property_reply_t *reply
        = xcb_get_property_reply(window->display->x11, cookie, NULL);
    if (!reply)
        return NULL;
    const int length = xcb_get_property_value_length(reply);
    if (length == 0) {
        free(reply);
        return NULL;
    }
    char *value = xcb_get_property_value(reply);
    free(reply);
    return value;
}

int sfgl_get_window_width(sfgl_window_t *window)
{
    if (sfgl_should_window_close(window))
        return -1;
    return window->width;
}

int sfgl_get_window_height(sfgl_window_t *window)
{
    if (sfgl_should_window_close(window))
        return -1;
    return window->height;
}

int sfgl_get_window_x(sfgl_window_t *window)
{
    if (sfgl_should_window_close(window))
        return -1;
    return window->x;
}

int sfgl_get_window_y(sfgl_window_t *window)
{
    if (sfgl_should_window_close(window))
        return -1;
    return window->y;
}

bool sfgl_should_window_close(sfgl_window_t *window)
{
    return !window || window->should_close;
}

void sfgl_free_window(sfgl_window_t *window)
{
    if (!window)
        return;
    xcb_disconnect(window->display->x11);
    free(window->display->x11);
    free(window->display);
    free(window->close_atom);
    free(window);
}
