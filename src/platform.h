#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file specifies the API for any functionality provided by the host
// platform. The function signatures only appear here - each implementation
// should use the macros below when a signature is required.

#include "shared.h"

// NOTE: Output a printf-style formatted string to some reasonable location for
// a given platform.
#define PLATFORM_LOG(name) void name(char *fmt, ...)

// NOTE: Allocate the requested number of bytes. Cleared to zero. This is
// intended for bulk-allocations at startup, it makes no promises of being fast.
#define PLATFORM_ALLOCATE(name) void *name(memsize size)

// NOTE: Deallocate memory previous allocated by the platform API. This is
// mainly intended for undoing bulk-allocations at startup, it makes no
// promises of being fast.
#define PLATFORM_DEALLOCATE(name) void name(void *memory)


// NOTE: These expand to forward declarations of the function signatures above,
// in case the macro expansions are confusing. By convention, platform functions
// begin with the letter p.
PLATFORM_LOG(plog);
PLATFORM_ALLOCATE(pallocate);
PLATFORM_DEALLOCATE(pdeallocate);
