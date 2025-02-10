#include "felidae/common/payload.h"
#include <stdio.h>

static const char *const FELIDAE_PAYLOAD_MESSAGES[]
    = { [SUCCESS] = "Operation completed successfully",
        [MISSING_API_EXTENSIONS] = "Required API extensions are not available",
        [FAILED_TO_CONNECT_TO_DISPLAY_SERVER]
        = "Could not establish connection to display server",
        [FAILED_TO_INITIALIZE_EGL] = "Failed to initialize EGL",
        [FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG]
        = "No compatible EGL configuration found",
        [FAILED_TO_GET_VISUAL_ID] = "Could not obtain visual ID",
        [FAILED_TO_INITIALIZE_COLORMAP] = "Failed to initialize colormap",
        [FAILED_TO_CREATE_WINDOW] = "Window creation failed",
        [FAILED_TO_CHANGE_WINDOW_ATTRIBUTES]
        = "Failed to override window attributes",
        [FAILED_TO_CREATE_WINDOW_SURFACE] = "Failed to create window surface",
        [OUTDATED_EGL] = "EGL version is outdated",
        [FAILED_TO_BIND_OPENGL] = "Failed to bind OpenGL context",
        [CANT_OPEN_DISPLAY] = "Could not open display",
        [OUT_OF_MEMORY] = "Out of memory",
        [FAILED_TO_COMPILE_SHADERS] = "Failed to compile OpenGL shaders" };

felidae_payload_result felidae_make_payload_result(
    enum felidae_payload_result_kind kind, char *context
)
{
    if (!context)
        return (felidae_payload_result) { .kind = kind, .context = context };
    char buffer[256];
    snprintf(
        buffer, sizeof(buffer), "%s: %s", FELIDAE_PAYLOAD_MESSAGES[kind],
        context
    );
    return (felidae_payload_result) { .kind = kind, .context = buffer };
}

felidae_payload_result
felidae_decontextualized(enum felidae_payload_result_kind kind)
{
    return felidae_make_payload_result(kind, NULL);
}

felidae_payload_result felidae_success(void)
{
    return felidae_decontextualized(SUCCESS);
}
