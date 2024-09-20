#include "sfgl/window.h"
#include "sfgl/payload.h"
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

void sfgl_window_show(sfgl_window_t *win)
{
    xcb_map_window(win->config->display->x11, win->x11);
    xcb_flush(win->config->display->x11);
}

void sfgl_window_hide(sfgl_window_t *win)
{
    xcb_unmap_window(win->config->display->x11, win->x11);
    xcb_flush(win->config->display->x11);
}

#define update(MASK, FIELD)                                                    \
    {                                                                          \
        mask |= (MASK);                                                        \
        values.FIELD = *payload.FIELD;                                         \
        window->FIELD = *payload.FIELD;                                        \
    }

void sfgl_window_update(
    sfgl_window_t *window, sfgl_update_window_payload payload
)
{
    if (sfgl_window_was_closed(window))
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
    xcb_configure_window_aux(
        window->config->display->x11, window->x11, mask, &values
    );
    if (payload.title) {
        xcb_change_property(
            window->config->display->x11, XCB_PROP_MODE_REPLACE, window->x11,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(payload.title),
            payload.title
        );
    }
    xcb_flush(window->config->display->x11);
}

char *sfgl_window_get_title(sfgl_window_t *window)
{
    if (sfgl_window_was_closed(window))
        return NULL;
    const xcb_get_property_cookie_t cookie = xcb_get_property(
        window->config->display->x11, 0, window->x11, XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING, 0, 0
    );
    xcb_get_property_reply_t *reply
        = xcb_get_property_reply(window->config->display->x11, cookie, NULL);
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

int sfgl_window_get_width(sfgl_window_t *window)
{
    if (sfgl_window_was_closed(window))
        return 0;
    return window->width;
}

int sfgl_window_get_height(sfgl_window_t *window)
{
    if (sfgl_window_was_closed(window))
        return 0;
    return window->height;
}

int sfgl_window_get_x(sfgl_window_t *window)
{
    if (sfgl_window_was_closed(window))
        return 0;
    return window->x;
}

int sfgl_window_get_y(sfgl_window_t *window)
{
    if (sfgl_window_was_closed(window))
        return 0;
    return window->y;
}

bool sfgl_window_was_closed(sfgl_window_t *window)
{
    return !window || window->was_close_requested;
}

void sfgl_free(sfgl_window_t *window)
{
    if (!window)
        return;
    xcb_disconnect(window->config->display->x11);
    free(window->config->display);
    free(window->config);
    free(window);
}
