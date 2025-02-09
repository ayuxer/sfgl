#include "felidae/opengl/context.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "felidae/common/payload.h"
#include "felidae/opengl/wrapper.h"
#include "felidae/windowing/core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

felidae_payload_result felidae_check_api_extensions(void)
{
    const char *client_extensions
        = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!client_extensions
        || !strstr(client_extensions, "EGL_EXT_platform_xcb")) {
        return felidae_decontextualized(MISSING_API_EXTENSIONS);
    }
    return felidae_success();
}

felidae_payload_result felidae_opengl_init(felidae_init_opengl_payload payload)
{
    EGLint major, minor;
    if (!eglInitialize(payload.egl, &major, &minor)) {
        return felidae_decontextualized(FAILED_TO_INITIALIZE_EGL);
    }
    if (major < payload.min_major_egl_version
        || minor < payload.min_minor_egl_version) {
        return felidae_decontextualized(OUTDATED_EGL);
    }
    if (!eglBindAPI(EGL_OPENGL_API)) {
        return felidae_decontextualized(FAILED_TO_BIND_OPENGL);
    }

    return felidae_success();
}

felidae_payload_result felidae_graphics_create_context(
    felidae_graphics_context_t **ctx, felidae_window_t *window
)
{
    felidae_payload_result res = felidae_check_api_extensions();
    if (res.kind)
        return res;

    *ctx = malloc(sizeof(felidae_graphics_context_t));
    if (*ctx == NULL)
        return felidae_decontextualized(OUT_OF_MEMORY);

    (*ctx)->display = eglGetPlatformDisplay(
        EGL_PLATFORM_XCB_EXT, window->display->x11,
        (const EGLAttrib[]) { EGL_PLATFORM_XCB_SCREEN_EXT,
                              felidae_get_window_id(window->display), EGL_NONE }
    );
    if ((*ctx)->display == EGL_NO_DISPLAY) {
        felidae_opengl_free(*ctx);
        return felidae_decontextualized(CANT_OPEN_DISPLAY);
    }

    res = felidae_opengl_init((felidae_init_opengl_payload
    ) { .egl = (*ctx)->display,
        .min_major_egl_version = EGL_MIN_VER_MINOR,
        .min_minor_egl_version = EGL_MIN_VER_MAJOR });

    if (res.kind)
        return res;

    // clang-format off
    const EGLint egl_config_attribs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                          EGL_RED_SIZE, 8,
                                          EGL_GREEN_SIZE, 8,
                                          EGL_BLUE_SIZE, 8,
                                          EGL_ALPHA_SIZE, 8,
                                          EGL_DEPTH_SIZE, 24,
                                          EGL_STENCIL_SIZE, 8,
                                          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
                                          EGL_CONFORMANT, EGL_OPENGL_BIT,
                                          EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
                                          EGL_NONE };
    // clang-format on

    EGLint num_configs;

    if (!eglChooseConfig(
            (*ctx)->display, egl_config_attribs, &(*ctx)->config, 1,
            &num_configs
        )) {
        felidae_opengl_free(*ctx);
        return felidae_decontextualized(FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG);
    }
    if (num_configs == 0) {
        felidae_opengl_free(*ctx);
        return felidae_decontextualized(FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG);
    }

    EGLAttrib surface_attributes[] = {
        EGL_GL_COLORSPACE, EGL_GL_COLORSPACE_LINEAR,
        EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
        EGL_NONE,
    };
    (*ctx)->surface = eglCreatePlatformWindowSurface(
        (*ctx)->display, (*ctx)->config, &window->x11, surface_attributes
    );
    if ((*ctx)->surface == EGL_NO_SURFACE) {
        felidae_opengl_free(*ctx);
        return felidae_decontextualized(FAILED_TO_CREATE_WINDOW_SURFACE);
    }

    (*ctx)->egl = eglCreateContext(
        (*ctx)->display, (*ctx)->config, EGL_NO_CONTEXT,
        (const EGLint[]) { EGL_CONTEXT_MAJOR_VERSION, OPENGL_MIN_VER_MAJOR,
                           EGL_CONTEXT_MINOR_VERSION, OPENGL_MIN_VER_MINOR,
                           EGL_CONTEXT_OPENGL_PROFILE_MASK, OPENGL_PROFILE,
                           EGL_NONE }
    );
    if ((*ctx)->egl == EGL_NO_CONTEXT) {
        felidae_opengl_free(*ctx);
        return felidae_decontextualized(FAILED_TO_INITIALIZE_EGL);
    }

    if (!eglMakeCurrent(
            (*ctx)->display, (*ctx)->surface, (*ctx)->surface, (*ctx)->egl
        )) {
        felidae_opengl_free(*ctx);
        return felidae_decontextualized(FAILED_TO_INITIALIZE_EGL);
    }

    return felidae_graphics_init(window, *ctx);
}

void felidae_opengl_free(struct felidae_graphics_context *ctx)
{
    if (ctx == NULL)
        return;
    if (ctx->display != EGL_NO_DISPLAY) {
        if (ctx->surface != EGL_NO_SURFACE)
            eglDestroySurface(ctx->display, ctx->surface);
        if (ctx->egl != EGL_NO_CONTEXT)
            eglDestroyContext(ctx->display, ctx->egl);
        eglTerminate(ctx->display);
    }
    free(ctx);
}
