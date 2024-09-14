#pragma once

#include "gb8x8.h"

typedef struct GLFWwindow GLFWwindow;

bool    graphics_initialize(GLFWwindow* context);
void    graphics_terminate();

u32  shader_create(char const *vertex_source, char const *fragment_source);
void shader_destroy(u32 id);

u32  texture_create(u32 width, u32 height);
void texture_destroy(u32 tex);

void texture_set_sub_image(u32 texture, u32 x, u32 y, u32 width, u32 height, const void* pixel_data);

void draw_quad(u32 texture_id, f32 x, f32 y, f32 width, f32 height);

