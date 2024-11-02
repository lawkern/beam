/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "game.h"
#include "platform.h"

#include "memory.cpp"

GAME_INITIALIZE(game_initialize)
{
   // NOTE: Initialize memory.
   game->perma = arena_new(MEGABYTES(64));
   game->frame = arena_new(MEGABYTES(64));
   if(game->perma.size == 0 || game->frame.size == 0)
   {
      return;
   }

   // NOTE: Initialize backbuffer.
   game_texture *backbuffer = &game->backbuffer;
   backbuffer->width = 960;
   backbuffer->height = 540;
   backbuffer->memory = arena_array(&game->perma, u32, backbuffer->width*backbuffer->height);
   if(!backbuffer->memory)
   {
      plog("ERROR: Failed to allocate the game backbuffer.\n");
      return;
   }

   // NOTE: Initialization was successful.
   game->running = true;
}

GAME_UPDATE(game_update)
{
   game_texture backbuffer = game->backbuffer;

   for(int y = 0; y < backbuffer.height; ++y)
   {
      for(int x = 0; x < backbuffer.width; ++x)
      {
         u32 color = 0x0000FFFF + ((x % 255) << 24);
         backbuffer.memory[backbuffer.width*y + x] = color;
      }
   }
}
