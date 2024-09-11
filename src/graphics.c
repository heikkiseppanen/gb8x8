#include "graphics.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

static u32 gl_create_shader_module(char const* source, GLenum type);

static void gl_debug_callback(
    GLenum       source,
    GLenum       type,
    GLuint       id,
    GLenum       severity,
    GLsizei      length,
    GLchar const *message,
    void const   *userParam);

i32 graphics_init() {
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        printf("Failed to initialize OpenGL context\n");
        return 0;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // glDebugMessageCallback(gl_debug_callback, NULL);

    GLFWwindow* context = glfwGetCurrentContext();
    if (!context) {
        printf("Failed to initialize graphics backend, no GLFW context set\n");
        return 0;
    }
    
    i32 width, height;
    glfwGetWindowSize(context, &width, &height);

    glViewport(0, 0, width, height);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    printf("OpenGL %d.%d initialized\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version)); 

    return 1;
}

u32 create_shader(const char* vertex_source, const char* fragment_source) {
    u32 shader = 0;
    u32 vert_module = 0;
    u32 frag_module = 0;

    i32 link_success = 0;

    if (!vertex_source || !fragment_source) {
        return 0;
    }

    vert_module = gl_create_shader_module(vertex_source, GL_VERTEX_SHADER);
    frag_module = gl_create_shader_module(vertex_source, GL_VERTEX_SHADER);

    if (!vert_module || !frag_module) {
        glDeleteShader(vert_module);
        glDeleteShader(frag_module);
        return 0;
    }

    shader = glCreateProgram();

    if (shader) {
        glAttachShader(shader, vert_module);
        glAttachShader(shader, frag_module);

        glLinkProgram(shader);

        glGetProgramiv(shader, GL_LINK_STATUS, &link_success);
        if (!link_success) {
            GLsizei len;
            GLchar err[256];
            glGetProgramInfoLog(shader, 256, &len, err);
            gl_debug_callback(
                GL_DEBUG_SOURCE_SHADER_COMPILER,
                GL_DEBUG_TYPE_ERROR,
                shader,
                GL_DEBUG_SEVERITY_HIGH,
                len, err, NULL);
            glDeleteProgram(shader);
            shader = 0;
        }
    }

    glDetachShader(shader, vert_module);
    glDetachShader(shader, frag_module);

    glDeleteShader(vert_module);
    glDeleteShader(frag_module);

    return shader;
}

void destroy_shader(shader_id id) {
    glDeleteProgram(id);
}

u32 gl_create_shader_module(const char* source, GLenum type) {
    u32 id = glCreateShader(type);

    if (!id) {
        return id;
    }

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int compilation_success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_success);

    if (!compilation_success) {
        GLsizei length;
        GLchar message[256];
        glGetShaderInfoLog(id, 256, &length, message);
        gl_debug_callback(
            GL_DEBUG_SOURCE_SHADER_COMPILER,
            GL_DEBUG_TYPE_ERROR,
            id,
            GL_DEBUG_SEVERITY_HIGH,
            length, message, NULL);
    }

    return id;
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

    printf("GL_%s_%s_%s ID: %d Info: %.*s", severity_str, src_str, type_str, id, length, message);
}
