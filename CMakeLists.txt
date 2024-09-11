cmake_minimum_required(VERSION 3.30)

project( 
    gb8x8
    VERSION 0.01
    LANGUAGES CXX C 
)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${SRC_DIR}/main.c
    ${SRC_DIR}/graphics.c
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${SRC_DIR}
)

add_subdirectory(${LIB_DIR})

target_compile_features(${PROJECT_NAME}
    PUBLIC
        c_std_17
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        $<$<CONFIG:Debug>: GB_DEBUG>
)

target_compile_options(${PROJECT_NAME}
    PUBLIC
        $<$<CONFIG:Debug>: -g -fno-omit-frame-pointer >
        $<$<CONFIG:Release>: -O3>
        -Wall -Werror -Wextra
)

target_link_options(
    ${PROJECT_NAME}
    PUBLIC
        $<$<CONFIG:Debug>: -g -fno-omit-frame-pointer >
)
