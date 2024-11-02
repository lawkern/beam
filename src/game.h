#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file specifies the API provided by the game. Anything defined here
// is visible to the platform implementations.

#include "shared.h"
#include "memory.h"

#define GAME_TEXTURE_SIZE(t) (sizeof(*((t).memory)) * (t).width * (t).height)

struct game_texture
{
   int width;
   int height;
   u32 *memory;
};

struct game_context
{
   game_texture backbuffer;

   memarena perma;
   memarena frame;

   bool running;
};

// NOTE: Initialize any game-specific state. This should be called once at the
// platform-defined entry point.
#define GAME_INITIALIZE(name) void name(game_context *game)

// NOTE: Update the game simulation. This should be called once per frame.
#define GAME_UPDATE(name) void name(game_context *game)


// NOTE: These expand to forward declarations of the function signatures above,
// in case the macro expansions are confusing. By convention, game functions
// begin with the prefix game_.
GAME_INITIALIZE(game_initialize);
GAME_UPDATE(game_update);
