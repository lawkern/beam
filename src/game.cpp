#include "game.h"
#include "platform.h"

GAME_INITIALIZE(game_initialize)
{
   // NOTE: Initialize backbuffer.
   game_texture *backbuffer = &game->backbuffer;
   backbuffer->width = 960;
   backbuffer->height = 540;

   memsize backbuffer_size = sizeof(*backbuffer->memory) * backbuffer->width * backbuffer->height;
   backbuffer->memory = (u32 *)pallocate(backbuffer_size);

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
