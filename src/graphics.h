#pragma once

#include "gb8x8.h"

typedef u32 shader_id;

typedef struct GLFWwindow GLFWwindow;

i32 graphics_init(GLFWwindow* context);

shader_id create_shader(char const *vertex_source, char const *fragment_source);
void      destroy_shader(shader_id id);
