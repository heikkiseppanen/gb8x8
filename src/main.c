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
//"   gl_Position = vec4(                                                      \n"
//"       0.0f + (float(gl_VertexID & 1) * 0.5f),                  \n"
//"       -1.0f + (float(gl_VertexID > 1) * 0.5f),                  \n"
//"       0.0f, 0.0f                                                            \n"
//"   ); 
"   vec2 positions[4] = vec2[](                                              \n"
"       vec2(u_extent.x + ((gl_VertexID & 1) * u_extent.z), u_extent.y),  // Top-left                                       \n"
"       vec2(u_extent.x + ((gl_VertexID & 1) * u_extent.z), u_extent.y),  // Top-right                                      \n"
"       vec2(u_extent.x + ((gl_VertexID & 1) * u_extent.z), u_extent.y + u_extent.w), // Bottom-left                                   \n"
"       vec2(u_extent.x + ((gl_VertexID & 1) * u_extent.z), u_extent.y + u_extent.w)   // Bottom-right                                    \n"
"   );                                                                       \n"
"   gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);                    \n"
"}                                                                           \n";

const char *frag_source = 
"#version 430                                                                \n"
"                                                                            \n"
"out vec4 frag_color;                                                        \n"
"                                                                            \n"
"void main() {                                                               \n"
//"   frag_color = texture(u_texture, gl_FragCoord);                         \n  "
"   frag_color = vec4(gl_FragCoord);                               \n"
"}                                                                           \n";

void glfw_error_callback(int err, const char* msg) {
    printf("GLFW error: %x %s\n", err, msg);
}

int main() {
    GLFWwindow *window   = NULL;
    GLFWmonitor *monitor = NULL;
    GLFWvidmode const *video_mode = NULL;

    shader_id display_shader   = 0;
    //texture_id display_texture = 0;

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

    display_shader = shader_create(vert_source, frag_source);

    // Application loop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        for (u32 cycle = 0; cycle < 35112; ++cycle) { // Approx 60fps 
            ppu_cycle(&picture_processing_unit);
        }

        draw_quad(0.0f, 0.0, 1.0f, 1.0f);

        glfwSwapBuffers(window);
    }

error:
    shader_destroy(display_shader);
    graphics_terminate();
    glfwTerminate();

    return 0;
}
