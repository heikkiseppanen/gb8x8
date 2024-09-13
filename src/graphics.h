#pragma once

#include "gb8x8.h"

typedef u32 shader_id;
typedef u32 texture_id;

typedef struct GLFWwindow GLFWwindow;

bool graphics_initialize(GLFWwindow* context);
void graphics_terminate();

shader_id shader_create(char const *vertex_source, char const *fragment_source);
void      shader_destroy(shader_id id);

void draw_quad(f32 x, f32 y, f32 width, f32 height);

//texture_id texture_create(u32 width, u32 height);
//void       texture_destroy(texture_id id);
