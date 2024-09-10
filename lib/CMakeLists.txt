cmake_minimum_required(VERSION 3.25)

set(GLFW_BUILD_TESTS    OFF CACHE BOOL "Lib only" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Lib only" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "Lib only" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "Lib only" FORCE)
add_subdirectory(glfw)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        glfw/include
        glad
        nuklear
)



