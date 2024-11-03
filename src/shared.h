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

#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#define MINIMUM(a, b) ((a) < (b) ? (a) : (b))
