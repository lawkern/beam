/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "game.h"
#include "platform.h"

#include "memory.cpp"
#include "math.cpp"
#include "assets.cpp"
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

   // NOTE: Load assets.
   game->debug_mesh.vertex_count = countof(cube_vertices);
   game->debug_mesh.vertices = cube_vertices;
   game->debug_mesh.texcoord_count = countof(cube_texcoords);
   game->debug_mesh.texcoords = cube_texcoords;
   game->debug_mesh.face_count = countof(cube_faces);
   game->debug_mesh.faces = cube_faces;

   // NOTE: Initialization was successful.
   game->running = true;
}

GAME_UPDATE(game_update)
{
   game_texture backbuffer = game->backbuffer;
   memarena *perma = &game->perma;
   memarena *frame = &game->frame;

   push_clear(game, 0x333333FF);

   // NOTE: Test basic triangle drawing.
   int debug_triangle_count = 20;
   for(int index = 0; index < debug_triangle_count; ++index)
   {
      assert(game->triangle_count < game->triangle_count_max);
      int triangle_index = game->triangle_count++;

      render_triangle *triangle = game->triangles + triangle_index;
      triangle->color = 0x00FF00FF;

      vec2i origin = {80, 80};
      int half_dim = 35;
      int offset = index * 10;

      triangle->vertices[0].x = offset + origin.x;
      triangle->vertices[0].y = offset + origin.y - half_dim;

      triangle->vertices[1].x = offset + origin.x - half_dim;
      triangle->vertices[1].y = offset + origin.y + half_dim;

      triangle->vertices[2].x = offset + origin.x + half_dim;
      triangle->vertices[2].y = offset + origin.y + half_dim;

      push_triangle(game, triangle_index);
   }

   // NOTE: Test basic mesh drawing.
   mesh_asset mesh = game->debug_mesh;

   game->scale.x = 100.0f;
   game->scale.y = 100.0f;
   game->scale.z = 100.0f;

   game->translation.x = backbuffer.width/2.0f;
   game->translation.y = backbuffer.height/2.0f;

   game->rotation.x += 0.01f;
   game->rotation.y += 0.01f;
   game->rotation.z += 0.01f;

   mat4 world = make_identity();
   world *= make_translation(game->translation.x, game->translation.y, game->translation.z);
   world *= make_rotationx(game->rotation.x);
   world *= make_rotationy(game->rotation.y);
   world *= make_rotationz(game->rotation.z);
   world *= make_scale(game->scale.x, game->scale.y, game->scale.z);

   for(int face_index = 0; face_index < mesh.face_count; ++face_index)
   {
      mesh_asset_face face = mesh.faces[face_index];

      assert(game->triangle_count < game->triangle_count_max);
      int triangle_index = game->triangle_count++;

      render_triangle *triangle = game->triangles + triangle_index;
      triangle->color = face.color;

      vec3 v0 = mesh.vertices[face.vertex_indices[0]];
      triangle->vertices[0] = mul(world, v4(v0, 1.0f)).xyz;

      vec3 v1 = mesh.vertices[face.vertex_indices[1]];
      triangle->vertices[1] = mul(world, v4(v1, 1.0f)).xyz;

      vec3 v2 = mesh.vertices[face.vertex_indices[2]];
      triangle->vertices[2] = mul(world, v4(v2, 1.0f)).xyz;

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
