#include "gb8x8.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "graphics.h"

#include "ppu.h"

const char *vert_source = 
"#version 430                                                                \n"
"                                                                            \n"
"layout(location = 0) uniform vec4 u_extent;                                 \n"
"                                                                            \n"
"out vec2 uv;                                                                \n"
"                                                                            \n"
"void main() {                                                               \n"
"   const float left_vert   = float(gl_VertexID & 1);                        \n"
"   const float bottom_vert = float(gl_VertexID > 1);                        \n"
"                                                                            \n"
"   gl_Position = vec4(                                                      \n"
"       u_extent.x + (left_vert   * u_extent.z),                             \n"
"       u_extent.y + (bottom_vert * u_extent.w),                             \n"
"       0.0f, 1.0f                                                           \n"
"   );                                                                       \n"
"                                                                            \n"
"   uv = vec2(left_vert, bottom_vert);                                       \n"
"}                                                                           \n";

const char *frag_source = 
"#version 430                                                                \n"
"                                                                            \n"
"layout(location = 1) uniform sampler2D u_texture;                           \n"
"                                                                            \n"
"in vec2 uv;                                                                 \n"
"                                                                            \n"
"out vec4 frag_color;                                                        \n"
"                                                                            \n"
"void main() {                                                               \n"
"   frag_color = vec4(texture(u_texture, uv).xyz, 1.0f);                                     \n"
//"   frag_color = vec4(uv, 0.0, 1.0f);                                        \n"
"}                                                                           \n";

void glfw_error_callback(int err, const char* msg) {
    printf("GLFW error: %x %s\n", err, msg);
}

int main() {
    GLFWwindow *window   = NULL;
    GLFWmonitor *monitor = NULL;
    GLFWvidmode const *video_mode = NULL;

    u32 display_shader  = 0;
    u32 display_texture = 0;

    glfwSetErrorCallback((GLFWerrorfun)glfw_error_callback);

    if (!glfwInit()){
        goto error;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    monitor    = glfwGetPrimaryMonitor();
    video_mode = glfwGetVideoMode(monitor);
    if (video_mode) {
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
    }

    window = glfwCreateWindow(LCD_WIDTH * 2, LCD_HEIGHT * 2, "gb8x8", NULL, NULL);
    if (!window) {
        goto error;
    }

    glfwMakeContextCurrent(window);

    if (!graphics_initialize(window)) {
        goto error;
    }

    ppu picture_processing_unit = {
        .stat = PPU_MODE_DRAWING & LCD_STATUS_PPU_MODE_MASK
    };

    display_shader  = shader_create(vert_source, frag_source);
    display_texture = texture_create(LCD_WIDTH, LCD_HEIGHT);

    // Application loop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        for (u32 cycle = 0; cycle < 35112; ++cycle) { // Approx 60fps 
            ppu_cycle(&picture_processing_unit);
        }

        draw_quad(display_texture, 0.0f, 0.0, 2.0f, 2.0f);

        glfwSwapBuffers(window);
    }

error:
    texture_destroy(display_texture);
    shader_destroy(display_shader);
    graphics_terminate();
    glfwTerminate();

    return 0;
}
