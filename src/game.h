#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file specifies the API provided by the game. Anything defined here
// is visible to the platform implementations.

#include "shared.h"

struct game_texture
{
   int width;
   int height;
   u32 *memory;
};
