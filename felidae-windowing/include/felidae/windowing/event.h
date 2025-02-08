#ifndef FELIDAE_EVENT_H
#define FELIDAE_EVENT_H

#include "felidae/windowing/core.h"

enum felidae_event_kind {
    KEY_PRESS = 1,
    KEY_RELEASE,
    RESIZE,
};

struct felidae_key_press_data {
    int key_code;
};

struct felidae_key_release_data {
    int key_code;
};

struct felidae_resize_data {
    int new_width;
    int new_height;
};

union felidae_event_data {
    struct felidae_key_press_data key_press;
    struct felidae_key_release_data key_release;
    struct felidae_resize_data resize;
};

struct felidae_event {
    unsigned int timestamp;
    enum felidae_event_kind kind;
    union felidae_event_data data;
};

struct felidae_event *felidae_event_poll(felidae_window_t *window);

#endif
