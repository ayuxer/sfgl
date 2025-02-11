#include "felidae/opengl/camera.h"

#include <cglm/cglm.h>

static constexpr vec3 UP = { 0.f, 1.f, 0.f };
static constexpr vec3 FRONT = { 0.f, 0.f, -1.f };
static constexpr vec3 SIDEWAYS = { 1.f, -0.f, 0.f };

felidae_camera_view felidae_create_camera_view(vec3 position, float fov)
{
    felidae_camera_view view = {
        .fov = fov,
    };
    glm_vec3_copy(position, view.position);
    return view;
}

void felidae_camera_build_mvp(
    felidae_camera_view *view, float aspect_ratio, mat4 dest
)
{
    mat4 projection, p_view, model = GLM_MAT4_IDENTITY_INIT;
    vec3 center;

    glm_vec3_add(view->position, (float *)FRONT, center);
    glm_lookat_rh_no(view->position, center, (float *)UP, p_view);
    glm_perspective(view->fov, aspect_ratio, .1f, 100.f, projection);
    glm_rotate(model, view->model_angle, (float[3]) { 1.f, 0.f, 0.f });
    glm_mat4_mulN((mat4 *[3]) { &projection, &p_view, &model }, 3, dest);
}

void felidae_camera_turn_left(felidae_camera_view *view, float scale)
{
    glm_vec3_mulsubs((float *)SIDEWAYS, scale, view->position);
}

void felidae_camera_turn_right(felidae_camera_view *view, float scale)
{
    glm_vec3_muladds((float *)SIDEWAYS, scale, view->position);
}

void felidae_camera_turn_up(felidae_camera_view *view, float scale)
{
    glm_vec3_muladds((float *)FRONT, scale, view->position);
}

void felidae_camera_turn_down(felidae_camera_view *view, float scale)
{
    glm_vec3_mulsubs((float *)FRONT, scale, view->position);
}
