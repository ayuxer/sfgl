#ifndef FELIDAE_COMMON_PAYLOAD_H
#define FELIDAE_COMMON_PAYLOAD_H

enum felidae_payload_result_kind {
    SUCCESS = 0,
    MISSING_API_EXTENSIONS = 1,
    FAILED_TO_CONNECT_TO_DISPLAY_SERVER = 2,
    FAILED_TO_INITIALIZE_EGL = 3,
    FAILED_TO_FIND_COMPATIBLE_EGL_CONFIG = 4,
    FAILED_TO_GET_VISUAL_ID = 5,
    FAILED_TO_INITIALIZE_COLORMAP = 6,
    FAILED_TO_CREATE_WINDOW = 7,
    FAILED_TO_CHANGE_WINDOW_ATTRIBUTES = 8,
    FAILED_TO_CREATE_WINDOW_SURFACE = 9,
    OUTDATED_EGL = 10,
    FAILED_TO_BIND_OPENGL = 11,
    CANT_OPEN_DISPLAY = 12,
    OUT_OF_MEMORY = 13,
    FAILED_TO_COMPILE_SHADERS = 14,
};

typedef struct {
    int screen_idx;
} felidae_get_preferred_display_payload;

typedef struct {
    const char *title;
    int width, height;
    int x, y;
} felidae_create_window_payload;

typedef struct {
    const char *title;
    unsigned int *width;
    unsigned int *height;
} felidae_update_window_payload;

typedef struct {
    enum felidae_payload_result_kind kind;
    const char *context;
} felidae_payload_result;

felidae_payload_result felidae_make_payload_result(
    enum felidae_payload_result_kind kind, char *context
);

felidae_payload_result felidae_success(void);

felidae_payload_result
felidae_decontextualized(enum felidae_payload_result_kind kind);

#endif
