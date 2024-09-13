#include "graphics.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

static GLuint g_dummy_vertex_array_object = 0;

static u32 gl_create_shader_module(char const* source, GLenum type);

static void gl_debug_callback(
    GLenum       source,
    GLenum       type,
    GLuint       id,
    GLenum       severity,
    GLsizei      length,
    GLchar const *message,
    void const   *userParam);

bool graphics_initialize(GLFWwindow* context) {
    GB_ASSERT(context != NULL, "Invalid OpenGL context!");
    
    const int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        printf("Failed to load OpenGL\n");
        return false;
    }

    // TODO Lock down OpenGL version
    if (version >= GLAD_MAKE_VERSION(4, 3)) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_callback, NULL);
    }

    glEnable(GL_FRAMEBUFFER_SRGB);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &g_dummy_vertex_array_object);
    glBindVertexArray(g_dummy_vertex_array_object);

    if (!g_dummy_vertex_array_object) {
        GB_ERROR("Failed to create dummy VAO for attributeless rendering");
        return false;
    }


    i32 width, height;
    glfwGetWindowSize(context, &width, &height);

    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    GB_INFO("OpenGL %d.%d initialized", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version)); 

    return true;
}

void graphics_terminate() {
    glDeleteVertexArrays(1, &g_dummy_vertex_array_object);
}

u32 shader_create(const char* vertex_source, const char* fragment_source) {
    u32 id = {};
    u32 vert_module = 0;
    u32 frag_module = 0;
    i32 link_success = 0;

    if (!vertex_source || !fragment_source) {
        return 0;
    }

    vert_module = gl_create_shader_module(vertex_source,   GL_VERTEX_SHADER);
    frag_module = gl_create_shader_module(fragment_source, GL_FRAGMENT_SHADER);

    if (!vert_module || !frag_module) {
        glDeleteShader(vert_module);
        glDeleteShader(frag_module);
        return 0;
    }

    id = glCreateProgram();

    if (id) {
        glAttachShader(id, vert_module);
        glAttachShader(id, frag_module);

        glLinkProgram(id);

        glGetProgramiv(id, GL_LINK_STATUS, &link_success);
        if (!link_success) {
            GLsizei len;
            GLchar err[256];
            glGetProgramInfoLog(id, 256, &len, err);
            gl_debug_callback(
                GL_DEBUG_SOURCE_SHADER_COMPILER,
                GL_DEBUG_TYPE_ERROR,
                id,
                GL_DEBUG_SEVERITY_HIGH,
                len, err, NULL);
            glDeleteProgram(id);
            id = 0;
        }
    }

    glDetachShader(id, vert_module);
    glDetachShader(id, frag_module);

    glDeleteShader(vert_module);
    glDeleteShader(frag_module);

    glUseProgram(id);

    return id;
}

void shader_destroy(u32 id) {
    glDeleteProgram(id);
}

u32 texture_create(u32 width, u32 height) {
    u32 id = 0;

    (void) width;
    (void) height;
    glGenTextures(1, &id);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 160, 144, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL); 

    u32 arr[160] = {};

    u32 *it = arr;
    u32 *end = arr + 160;
    while (it != end) *(it++) = 0xFF0000FF;

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 144 -32, 160, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, arr);
    
    return id;
}

void texture_destroy(u32 texture) {
    glDeleteTextures(1, &texture);
}

void draw_quad(u32 tex, f32 x, f32 y, f32 width, f32 height) {
    glUniform4f(0, x * 2.0f - 1.0f, y * 2.0f - 1.0f, width, height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static u32 gl_create_shader_module(const char* source, GLenum type) {
    u32 module = glCreateShader(type);
    i32 compilation_success = 0;

    if (!module) {
        return module;
    }

    glShaderSource(module, 1, &source, NULL);
    glCompileShader(module);

    glGetShaderiv(module, GL_COMPILE_STATUS, &compilation_success);

    if (!compilation_success) {
        GLsizei length;
        GLchar message[256];
        glGetShaderInfoLog(module, 256, &length, message);
        gl_debug_callback(
            GL_DEBUG_SOURCE_SHADER_COMPILER,
            GL_DEBUG_TYPE_ERROR,
            module,
            GL_DEBUG_SEVERITY_HIGH,
            length, message, NULL);
        glDeleteShader(module); module = 0;
    }

    return module;
}

static void gl_debug_callback(
    GLenum       source,
    GLenum       type,
    GLuint       id,
    GLenum       severity,
    GLsizei      length,
    GLchar const *message,
    void const   *userParam
){
    (void)length;
    (void)userParam;

    const char* severity_str = NULL;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         severity_str = "::HIGH";    break;
        case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "::MEDIUM";  break;
        case GL_DEBUG_SEVERITY_LOW:          severity_str = "::LOW";     break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "::INFO";    break;
        default:                             severity_str = "::UNKNOWN"; break;
    }

    const char* src_str = NULL;
    switch (source) {
        case GL_DEBUG_SOURCE_API:             src_str = "API";       break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   src_str = "WINDOW";    break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: src_str = "SHADER";    break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     src_str = "3RD_PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     src_str = "USER";      break;
        case GL_DEBUG_SOURCE_OTHER:           src_str = "OTHER";     break;
        default:                              src_str = "UNKNOWN";   break;
    }
    
    const char* type_str = NULL;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               type_str = "ERROR";        break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "DEPRECATED";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "UNDEFINED";    break;
        case GL_DEBUG_TYPE_PORTABILITY:         type_str = "NON_PORTABLE"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "PERFORMANCE";  break;
        case GL_DEBUG_TYPE_MARKER:              type_str = "MARKET";       break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          type_str = "PUSH_GROUP";   break;
        case GL_DEBUG_TYPE_POP_GROUP:           type_str = "POP_GROUP";    break;
        case GL_DEBUG_TYPE_OTHER:               type_str = "OTHER";        break;
        default:                                type_str = "UNKNOWN";      break;
    }

    GB_ERROR("GL_%s_%s_%s ID: %d Info: %.*s", severity_str, src_str, type_str, id, length, message);
}
