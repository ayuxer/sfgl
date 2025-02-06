#include "sfgl/opengl/context.h"
#include "sfgl/payload.h"
#include "sfgl/window.h"
#include <EGL/eglext.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __sfgl_x11__
#include "unix/x11/window.c"
#endif

enum sfgl_payload_result sfgl_check_api_extensions(void)
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

enum sfgl_payload_result sfgl_opengl_init(sfgl_init_opengl_payload payload)
{
    EGLint major, minor;
    if (!eglInitialize(payload.egl, &major, &minor)) {
        fprintf(stderr, "Failed to initialize EGL.\n");
        return FAILED_TO_INITIALIZE_EGL;
    }
    if (major < payload.min_major_egl_version
        || minor < payload.min_minor_egl_version) {
        fprintf(
            stderr,
            "Expected at least EGL version %d.%d, found %d.%d instead.\n",
            payload.min_major_egl_version, payload.min_minor_egl_version, major,
            minor
        );
        return OUTDATED_EGL;
    }
    if (!eglBindAPI(EGL_OPENGL_API)) {
        fprintf(stderr, "Failed to bind EGL to OpenGL APIs.\n");
        return FAILED_TO_BIND_OPENGL;
    }
    return SUCCESS;
}

enum sfgl_payload_result sfgl_graphics_create_context(
    struct sfgl_graphics_context **ctx, sfgl_window_t *window
)
{
    *ctx = malloc(sizeof(struct sfgl_graphics_context));

    (*ctx)->display = eglGetPlatformDisplay(
        EGL_PLATFORM_XCB_EXT, window->display->x11,
        (const EGLAttrib[]) { EGL_PLATFORM_XCB_SCREEN_EXT,
                              window->display->screen_idx, EGL_NONE }
    );
    if ((*ctx)->display == EGL_NO_DISPLAY) {
        fprintf(stderr, "Failed to find a connected display.\n");
        free(*ctx);
        return CANT_OPEN_DISPLAY;
    }

    sfgl_opengl_init((sfgl_init_opengl_payload) { .egl = (*ctx)->display,
                                                  .min_major_egl_version = 1,
                                                  .min_minor_egl_version = 4 });

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
        fprintf(stderr, "Failed to choose a compatible EGL configuration.\n");
        free(*ctx);
        return FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG;
    }

    (*ctx)->egl = eglCreateContext(
        (*ctx)->display, (*ctx)->config, EGL_NO_CONTEXT,
        (const EGLint[]) { EGL_CONTEXT_MAJOR_VERSION, 2,
                           EGL_CONTEXT_MINOR_VERSION, 0, EGL_NONE }
    );
    if ((*ctx)->egl == EGL_NO_CONTEXT) {
        fprintf(stderr, "Failed to create EGL context.\n");
        free(*ctx);
        return FAILED_TO_INITIALIZE_EGL;
    }

    EGLint visualid = 0;
    if (!eglGetConfigAttrib(
            (*ctx)->display, (*ctx)->config, EGL_NATIVE_VISUAL_ID, &visualid
        )) {
        fprintf(stderr, "Failed to get Visual ID config attribute.\n");
        free(*ctx);
        return FAILED_TO_GET_VISUAL_ID;
    }

#ifdef __sfgl_x11__
    sfgl_x11_init_window_colormap(window, visualid);
#endif

    (*ctx)->surface = eglCreatePlatformWindowSurface(
        (*ctx)->display, (*ctx)->config, &window->x11, NULL
    );
    if ((*ctx)->surface == EGL_NO_SURFACE) {
        fprintf(stderr, "Failed to create EGL window surface\n.e");
        free(*ctx);
        return FAILED_TO_CREATE_WINDOW_SURFACE;
    }

    return SUCCESS;
}
