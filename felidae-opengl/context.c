#include "felidae/opengl/context.h"
#include "felidae/common/payload.h"
#include "felidae/windowing/core.h"
#include <EGL/eglext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EGL_MIN_VER_MINOR 1
#define EGL_MIN_VER_MAJOR 4

enum felidae_payload_result felidae_check_api_extensions(void)
{
    const char *client_extensions
        = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!client_extensions
        || !strstr(client_extensions, "EGL_EXT_platform_xcb")) {
        return MISSING_API_EXTENSIONS;
    }
    free((char *)client_extensions);
    return SUCCESS;
}

enum felidae_payload_result
felidae_opengl_init(felidae_init_opengl_payload payload)
{
    EGLint major, minor;
    if (!eglInitialize(payload.egl, &major, &minor)) {
        return FAILED_TO_INITIALIZE_EGL;
    }
    if (major < payload.min_major_egl_version
        || minor < payload.min_minor_egl_version) {
        return OUTDATED_EGL;
    }
    if (!eglBindAPI(EGL_OPENGL_API)) {
        return FAILED_TO_BIND_OPENGL;
    }
    return SUCCESS;
}

enum felidae_payload_result felidae_graphics_create_context(
    struct felidae_graphics_context **ctx, felidae_window_t *window
)
{
    *ctx = malloc(sizeof(struct felidae_graphics_context));

    (*ctx)->display = eglGetPlatformDisplay(
        EGL_PLATFORM_XCB_EXT, window->display->x11,
        (const EGLAttrib[]) { EGL_PLATFORM_XCB_SCREEN_EXT,
                              window->display->screen_idx, EGL_NONE }
    );
    if ((*ctx)->display == EGL_NO_DISPLAY) {
        free(*ctx);
        return CANT_OPEN_DISPLAY;
    }

    felidae_opengl_init((felidae_init_opengl_payload
    ) { .egl = (*ctx)->display,
        .min_major_egl_version = EGL_MIN_VER_MINOR,
        .min_minor_egl_version = EGL_MIN_VER_MAJOR });

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

    EGLint num_configs;
    if (!eglChooseConfig(
            (*ctx)->display, egl_config_attribs, &(*ctx)->config, 1,
            &num_configs
        )
        || num_configs == 0) {
        free(*ctx);
        return FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG;
    }

    (*ctx)->egl = eglCreateContext(
        (*ctx)->display, (*ctx)->config, EGL_NO_CONTEXT,
        (const EGLint[]) { EGL_CONTEXT_MAJOR_VERSION, 2,
                           EGL_CONTEXT_MINOR_VERSION, 0, EGL_NONE }
    );
    if ((*ctx)->egl == EGL_NO_CONTEXT) {
        free(*ctx);
        return FAILED_TO_INITIALIZE_EGL;
    }

    EGLint visualid = 0;
    if (!eglGetConfigAttrib(
            (*ctx)->display, (*ctx)->config, EGL_NATIVE_VISUAL_ID, &visualid
        )) {
        free(*ctx);
        return FAILED_TO_GET_VISUAL_ID;
    }

#ifdef __felidae_x11__
    felidae_x11_init_window_colormap(window, visualid);
#endif

    (*ctx)->surface = eglCreatePlatformWindowSurface(
        (*ctx)->display, (*ctx)->config, &window->x11, NULL
    );
    if ((*ctx)->surface == EGL_NO_SURFACE) {
        free(*ctx);
        return FAILED_TO_CREATE_WINDOW_SURFACE;
    }

    return SUCCESS;
}
