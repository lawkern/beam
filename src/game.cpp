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

   // NOTE: Initialize renderer.
   game->triangle_count_max = 1024;
   game->triangles = arena_array(&game->perma, render_triangle, game->triangle_count_max);
   if(!game->triangles)
   {
      plog("ERROR: Failed to allocate the triangle list.\n");
      return;
   }

   game->render_command_count_max = 1024;
   game->render_commands = arena_array(&game->perma, render_command, game->render_command_count_max);
   if(!game->render_commands)
   {
      plog("ERROR: Failed to allocate the render command list.\n");
      return;
   }

   // NOTE: Initialization was successful.
   game->running = true;
}

GAME_UPDATE(game_update)
{
   game_texture backbuffer = game->backbuffer;
   memarena *perma = &game->perma;
   memarena *frame = &game->frame;

   push_clear(game, 0x333333FF);

   int debug_triangle_count = 20;
   for(int index = 0; index < debug_triangle_count; ++index)
   {
      assert(game->triangle_count < game->triangle_count_max);
      int triangle_index = game->triangle_count++;
      render_triangle *triangle = game->triangles + triangle_index;

      triangle->color = 0x00FF00FF;

      vec2 mid = {backbuffer.width/2, backbuffer.height/2};
      int offset = (index - (debug_triangle_count / 2)) * 10;

      triangle->vertices[0].x = offset + mid.x;
      triangle->vertices[0].y = offset + mid.y - 100;

      triangle->vertices[1].x = offset + mid.x - 100;
      triangle->vertices[1].y = offset + mid.y + 100;

      triangle->vertices[2].x = offset + mid.x + 100;
      triangle->vertices[2].y = offset + mid.y + 100;

      push_triangle(game, triangle_index);
   }

   // NOTE: End of frame cleanup.
   arena_reset(frame);
}

GAME_RENDER(game_render)
{
   game_texture backbuffer = game->backbuffer;

   for(int index = 0; index < game->render_command_count; ++index)
   {
      render_command *command = game->render_commands + index;
      switch(command->kind)
      {
         case RENDERCOMMAND_CLEAR: {
            clear(backbuffer, command->color);
         } break;

         case RENDERCOMMAND_TRIANGLE: {
            assert(command->index < game->triangle_count);
            draw_triangle(backbuffer, game->triangles[command->index]);
         } break;
      }
   }

   // NOTE: Clear this frame's renderer state.
   game->render_command_count = 0;
   game->triangle_count = 0;
}
