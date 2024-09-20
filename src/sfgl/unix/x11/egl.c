#include "sfgl/egl.h"
#include "sfgl/payload.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#define FAIL(name, err)                                                        \
    (name) = NULL;                                                             \
    return (err)

static xcb_screen_t *get_screen(xcb_connection_t *c, int screen)
{
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(c));
    for (; iter.rem; --screen, xcb_screen_next(&iter))
        if (screen == 0)
            return iter.data;
    return NULL;
}

enum sfgl_payload_result sfgl_egl_get_preferred_display(
    sfgl_egl_display_t **display, sfgl_get_preferred_display_payload payload
)
{
    *display = malloc(sizeof(sfgl_egl_window_t));
    if (((*display)->x11 = xcb_connect(NULL, &payload.screen_idx)) == NULL) {
        fprintf(stderr, "Failed to connect to X11 display server.\n");
        FAIL(*display, FAILED_TO_CONNECT_TO_DISPLAY_SERVER);
    }
    (*display)->screen_idx = payload.screen_idx;
    (*display)->egl = eglGetPlatformDisplay(
        EGL_PLATFORM_XCB_EXT, (*display)->x11,
        (const EGLAttrib[]) { EGL_PLATFORM_XCB_SCREEN_EXT,
                              (*display)->screen_idx, EGL_NONE }
    );
    if ((*display)->egl == EGL_NO_DISPLAY) {
        fprintf(stderr, "Failed to find a connected display.\n");
        FAIL(*display, CANT_OPEN_DISPLAY);
    }
    (*display)->root_of_screen_idx
        = get_screen((*display)->x11, (*display)->screen_idx)->root;
    return SUCCESS;
}

enum sfgl_payload_result sfgl_egl_init(sfgl_init_egl_payload payload)
{
    EGLint major, minor;
    if (!eglInitialize(payload.egl, &major, &minor)) {
        fprintf(stderr, "Failed to initialize EGL.\n");
        return FAILED_TO_INITIALIZE_EGL;
    }
    if (major < payload.min_major_egl_version
        || minor < payload.min_minor_egl_version) {
        fprintf(
            stderr, "Expected at least EGL version %d.%d, found %d.%d instead.",
            payload.min_major_egl_version, payload.min_minor_egl_version, major,
            minor
        );
        return OUTDATED_EGL;
    }
    if (!eglBindAPI(EGL_OPENGL_API)) {
        fprintf(stderr, "Failed to bind EGL to OpenGL APIs.");
        return FAILED_TO_BIND_OPENGL;
    }
    return SUCCESS;
}

enum sfgl_payload_result
sfgl_egl_create_config(sfgl_egl_config_t **config, sfgl_egl_display_t *display)
{
    *config = malloc(sizeof(sfgl_egl_config_t));
    (*config)->display = display;

    const EGLint egl_config_attribs[] = { EGL_SURFACE_TYPE,
                                          EGL_WINDOW_BIT,
                                          EGL_RED_SIZE,
                                          8,
                                          EGL_GREEN_SIZE,
                                          8,
                                          EGL_BLUE_SIZE,
                                          8,
                                          EGL_ALPHA_SIZE,
                                          8,
                                          EGL_DEPTH_SIZE,
                                          24,
                                          EGL_RENDERABLE_TYPE,
                                          EGL_OPENGL_BIT,
                                          EGL_NONE };

    EGLint num_configs = 1;
    if (!eglChooseConfig(
            display->egl, egl_config_attribs, &(*config)->egl, 1, &num_configs
        )) {
        fprintf(stderr, "Failed to choose a compatible EGL configuration.\n");
        FAIL(*config, FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG);
    }
    if (num_configs == 0) {
        fprintf(stderr, "Failed to choose a compatible EGL configuration.\n");
        FAIL(*config, FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG);
    }

    eglCreateContext(
        display->egl, (*config)->egl, EGL_NO_CONTEXT,
        (const EGLint[]) { EGL_CONTEXT_MAJOR_VERSION, 2,
                           EGL_CONTEXT_MINOR_VERSION, 0, EGL_NONE }
    );

    return SUCCESS;
}

enum sfgl_payload_result sfgl_egl_create_window(
    sfgl_egl_window_t **window, sfgl_egl_config_t *config,
    sfgl_create_window_payload payload
)
{
    *window = malloc(sizeof(sfgl_egl_window_t));
    (*window)->config = config;
    (*window)->x11 = xcb_generate_id(config->display->x11);
    xcb_screen_t *screen
        = get_screen(config->display->x11, config->display->screen_idx);

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
        config->display->x11,
        xcb_create_window_checked(
            config->display->x11, XCB_COPY_FROM_PARENT, (*window)->x11,
            config->display->root_of_screen_idx, payload.x, payload.y,
            payload.width, payload.height, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT,
            XCB_COPY_FROM_PARENT, value_mask, value_list
        )
    );
    (*window)->x = payload.x;
    (*window)->y = payload.y;
    (*window)->width = payload.width;
    (*window)->height = payload.height;

    if (err) {
        free(err);
        fprintf(stderr, "Failed to create X11 window\n.");
        FAIL(*window, FAILED_TO_CREATE_WINDOW);
    }

    EGLint visualid = 0;
    if (!eglGetConfigAttrib(
            config->display->egl, config->egl, EGL_NATIVE_VISUAL_ID, &visualid
        )) {
        fprintf(stderr, "Failed to get Visual ID config attribute.\n");
        FAIL(*window, FAILED_TO_GET_VISUAL_ID);
    }

    (*window)->colormap = xcb_generate_id(config->display->x11);

    err = xcb_request_check(
        config->display->x11,
        xcb_create_colormap_checked(
            config->display->x11, XCB_COLORMAP_ALLOC_NONE, (*window)->colormap,
            config->display->root_of_screen_idx, visualid
        )
    );
    if (err) {
        free(err);
        fprintf(stderr, "Failed to initialize X11 colormap.\n");
        FAIL(*window, FAILED_TO_INITIALIZE_COLORMAP);
    }

    err = xcb_request_check(
        config->display->x11,
        xcb_change_window_attributes_checked(
            config->display->x11, (*window)->x11, XCB_CW_COLORMAP,
            (const unsigned int[]) { (*window)->colormap }
        )
    );
    if (err) {
        free(err);
        fprintf(stderr, "Failed to override X11 window attributes.\n");
        FAIL(*window, FAILED_TO_CHANGE_WINDOW_ATTRIBUTES);
    }

    (*window)->egl = eglCreatePlatformWindowSurface(
        config->display->egl, config->egl, &(*window)->x11, NULL
    );
    if ((*window)->egl == EGL_NO_SURFACE) {
        fprintf(stderr, "Failed to create EGL window surface.");
        FAIL(*window, FAILED_TO_CREATE_WINDOW_SURFACE);
    }

    if (payload.title) {
        xcb_change_property(
            config->display->x11, XCB_PROP_MODE_REPLACE, (*window)->x11,
            XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(payload.title),
            payload.title
        );
    }

    auto proto_cookie
        = xcb_intern_atom(config->display->x11, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *proto_reply
        = xcb_intern_atom_reply(config->display->x11, proto_cookie, 0);
    auto close_cookie
        = xcb_intern_atom(config->display->x11, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *close_reply
        = xcb_intern_atom_reply(config->display->x11, close_cookie, 0);
    xcb_change_property(
        config->display->x11, XCB_PROP_MODE_REPLACE, (*window)->x11,
        proto_reply->atom, 4, 32, 1, &close_reply->atom
    );
    (*window)->close_atom = close_reply;

    return SUCCESS;
}

enum sfgl_payload_result sfgl_egl_check_api_extensions(void)
{
    const char *client_extensions
        = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!client_extensions
        || !strstr(client_extensions, "EGL_EXT_platform_xcb")) {
        fprintf(stderr, "Missing `EGL_EXT_platform_xcb` OpenGL extension.\n");
        return MISSING_API_EXTENSIONS;
    }
    return SUCCESS;
}
