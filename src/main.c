#include "gb8x8.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "graphics.h"

#include "ppu.h"

void glfw_error_callback(int err, const char* msg) {
    printf("GLFW error: %x %s\n", err, msg);
}

int main() {
    GLFWwindow *window   = NULL;
    GLFWmonitor *monitor = NULL;
    GLFWvidmode const *video_mode = NULL;

    glfwSetErrorCallback((GLFWerrorfun)glfw_error_callback);

    if (!glfwInit()){
        goto error;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

    monitor    = glfwGetPrimaryMonitor();
    video_mode = glfwGetVideoMode(monitor);
    if (video_mode) {
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
    }

    window = glfwCreateWindow(512, 512, "gb8x8", NULL, NULL);
    if (!window) {
        goto error;
    }

    glfwMakeContextCurrent(window);

    if (!graphics_init(window)) {
        goto error;
    }

    ppu picture_processing_unit = {
        .stat = PPU_MODE_DRAWING & LCD_STATUS_PPU_MODE_MASK
    };

    // Application loop
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        ppu_cycle(&picture_processing_unit);

        glfwSwapBuffers(window);
    }

error:
    glfwTerminate();

    return 0;
}
