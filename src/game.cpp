/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "game.h"
#include "platform.h"

#include "memory.cpp"
#include "math.cpp"
#include "render.cpp"

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

   clear(backbuffer, 0x333333FF);

   vec2 mid = {backbuffer.width/2, backbuffer.height/2};

   render_triangle triangle = {};
   triangle.color = 0x00FF00FF;

   triangle.vertices[0].x = mid.x;
   triangle.vertices[0].y = mid.y - 100;

   triangle.vertices[1].x = mid.x - 100;
   triangle.vertices[1].y = mid.y + 100;

   triangle.vertices[2].x = mid.x + 100;
   triangle.vertices[2].y = mid.y + 100;

   draw_triangle(backbuffer, triangle);
}
