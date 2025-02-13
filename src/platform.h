#pragma once

/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */

// NOTE: This file specifies the API for any functionality provided by the host
// platform. The function signatures only appear here - each implementation
// should use the macros below when a signature is required.

#include "shared.h"
#include "game.h"

// NOTE: Output a printf-style formatted string to some reasonable location for
// a given platform.
#define PLATFORM_LOG(name) void name(const char *fmt, ...)

// NOTE: Allocate the requested number of bytes. Cleared to zero. This is
// intended for bulk-allocations at startup, it makes no promises of being fast.
#define PLATFORM_ALLOCATE(name) void *name(memsize size)

// NOTE: Deallocate memory previous allocated by the platform API. This is
// mainly intended for undoing bulk-allocations at startup, it makes no
// promises of being fast.
#define PLATFORM_DEALLOCATE(name) void name(void *memory)

#define PLATFORM_INITIALIZE(name) void name(int width, int height)

#define PLATFORM_FRAME_BEGIN(name) bool name(game_input *input)

#define PLATFORM_RENDER(name) void name(game_texture backbuffer)

#define PLATFORM_FRAME_END(name) void name(game_context *game)

// NOTE: These expand to forward declarations of the function signatures above,
// in case the macro expansions are confusing.
PLATFORM_LOG(platform_log);
PLATFORM_ALLOCATE(platform_allocate);
PLATFORM_DEALLOCATE(platform_deallocate);

PLATFORM_INITIALIZE(platform_initialize);
PLATFORM_FRAME_BEGIN(platform_frame_begin);
PLATFORM_RENDER(platform_render);
PLATFORM_FRAME_END(platform_frame_end);
