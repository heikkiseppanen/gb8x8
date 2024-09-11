#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef _MSC_VER
    #include <intrin.h>
    #define GB_RBEAK() __debugbreak()
#elif __linux__
    #include <signal.h>
    #define GB_BREAK() raise(SIGTRAP)
#endif

#define GB_TERM_BLACK   "\x1B[31m"
#define GB_TERM_RED     "\x1B[31m"
#define GB_TERM_GREEN   "\x1B[32m"
#define GB_TERM_YELLOW  "\x1B[33m"
#define GB_TERM_BLUE    "\x1B[34m"
#define GB_TERM_MAGENTA "\x1B[35m"
#define GB_TERM_CYAN    "\x1B[36m"
#define GB_TERM_WHITE   "\x1B[37m"
#define GB_TERM_DEFAULT "\x1B[39m"
#define GB_TERM_RESET   "\x1B[0m"

#define GB_FLOG(FD, COLOR, FORMAT, ...) fprintf(FD, COLOR FORMAT "\n" GB_TERM_RESET __VA_OPT__(,)__VA_ARGS__ )

#define GB_INFO(FORMAT, ...)  GB_FLOG(stdout, GB_TERM_DEFAULT, FORMAT, __VA_ARGS__)
#define GB_WARN(FORMAT, ...)  GB_FLOG(stderr, GB_TERM_YELLOW,  FORMAT, __VA_ARGS__)
#define GB_ERROR(FORMAT, ...) GB_FLOG(stderr, GB_TERM_RED,     FORMAT, __VA_ARGS__)

#ifdef GB_DEBUG
    #define GB_ASSERT(CONDITION, FORMAT, ...) do {                             \
        if (!(CONDITION)) {                                                    \
            GB_ERROR(FORMAT, __VA_ARGS__);                                     \
            GB_BREAK();                                                        \
        }                                                                      \
    } while(0)
#else
    #define GB_ASSERT(CONDITION, FORMAT, ...)
#endif

typedef int8_t   i8 ;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
 
typedef uint8_t  u8 ;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

