#ifndef SFGL_EVENT_H
#define SFGL_EVENT_H

#include "sfgl/window.h"

enum sfgl_event_kind {
    KEY_PRESS = 1,
    KEY_RELEASE,
};

struct sfgl_key_press_data {
    int key_code;
};

struct sfgl_key_release_data {
    int key_code;
};

union sfgl_event_data {
    struct sfgl_key_press_data key_press;
    struct sfgl_key_release_data key_release;
};

struct sfgl_event {
    unsigned int timestamp;
    enum sfgl_event_kind kind;
    union sfgl_event_data data;
};

struct sfgl_event *sfgl_event_poll(sfgl_window_t *window);

#endif
