#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#define TITLE "BEAM"

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#include <stddef.h>
typedef ptrdiff_t memsize;

#define countof(array) (memsize)(sizeof(array) / sizeof((array)[0]))

#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#define MINIMUM(a, b) ((a) < (b) ? (a) : (b))

// NOTE: Define debuggable assertions based on the compiler being used.
#define MSVC_ASSERT(expression) do { if(!(expression)) { __debugbreak(); } } while(0)
#define LLVM_ASSERT(expression) do { if(!(expression)) { __builtin_trap(); } } while(0)
#define JANK_ASSERT(expression) do { if(!(expression)) { *(volatile int *)0 = 0; } } while(0)

#if defined(_MSC_VER)
#   define assert MSVC_ASSERT
#elif defined(__GNUC__) || defined(__clang__)
#   define assert LLVM_ASSERT
#else
#   define assert JANK_ASSERT
#endif

#define NETWORKING_SUPPORTED 0
#define SERVER_HOST "localhost"
#define SERVER_PORT 2000
#define SERVER_PACKET_SIZE 512
