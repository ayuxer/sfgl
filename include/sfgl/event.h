#ifndef SFGL_EVENT_H
#define SFGL_EVENT_H

#include "sfgl/window.h"

enum sfgl_event_kind {
  KEY_PRESS = 1,
  KEY_RELEASE,
};

typedef struct {
  enum sfgl_event_kind kind;
  unsigned int timestamp;
  void *data;
} sfgl_event;

typedef struct {
  int key_code;  
} sfgl_key_press_event;

typedef struct {
  int key_code;
} sfgl_key_release_event;

typedef void (*main_loop_callback)(sfgl_window_t*, sfgl_event*);

void sfgl_event_start_main_loop(sfgl_window_t *window, main_loop_callback notify);

#endif
