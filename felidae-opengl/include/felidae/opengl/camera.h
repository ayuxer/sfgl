#ifndef FELIDAE_OPENGL_CAMERA_H
#define FELIDAE_OPENGL_CAMERA_H

#include <cglm/cglm.h>

typedef struct {
    vec3 position;
    float model_angle;
    float fov;
} felidae_camera_view;

felidae_camera_view felidae_create_camera_view(vec3 position, float fov);

void felidae_camera_build_mvp(
    felidae_camera_view *view, float aspect_ratio, mat4 dest
);

void felidae_camera_turn_left(felidae_camera_view *view, float scale);

void felidae_camera_turn_right(felidae_camera_view *view, float scale);

void felidae_camera_turn_up(felidae_camera_view *view, float scale);

void felidae_camera_turn_down(felidae_camera_view *view, float scale);

#endif
