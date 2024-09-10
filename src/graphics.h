#pragma once

#include "gb8x8.h"

typedef u32 shader_id;

i32 graphics_init();

shader_id create_shader(const char* vertex_source, const char* fragment_source);

void      destroy_shader(shader_id id);
