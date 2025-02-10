#ifndef FELIDAE_OPENGL_BATCH_H
#define FELIDAE_OPENGL_BATCH_H

#include "felidae/common/payload.h"
#include "felidae/opengl/graphics.h"

typedef struct {
    float columns[4][4];
} felidae_matrix;

typedef struct {
    float vertex_position[3];
    float vertex_color[4];
    float texture_coordinates[2];
} felidae_vertex;

typedef struct {
    unsigned int len, capacity;
    felidae_vertex *data;
} felidae_vertex_buffer;

typedef struct {
    felidae_gl_id texture;
    felidae_matrix mvp;
    felidae_gl_id texture_location, mvp_location;
} felidae_uniforms;

typedef struct {
    felidae_gl_id shader_program;
    felidae_gl_id vao, vbo;
    felidae_vertex_buffer buf;
    felidae_uniforms uniforms;
} felidae_batch_renderer;

felidae_payload_result felidae_init_batch_renderer(
    felidae_batch_renderer *renderer, unsigned int buffer_capacity
);

void felidae_flush_vertices(felidae_batch_renderer *renderer);

void felidae_set_texture(
    felidae_batch_renderer *renderer, felidae_gl_id texture
);

void felidae_push_vertex(
    felidae_batch_renderer *renderer, float x, float y, float z, float red,
    float green, float blue, float alpha, float u, float v
);

#endif
