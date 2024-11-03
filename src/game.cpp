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
   backbuffer->width = 640;
   backbuffer->height = 400;
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
   game->meshes[0].vertex_count = countof(cube_vertices);
   game->meshes[0].vertices = cube_vertices;
   game->meshes[0].texcoord_count = countof(cube_texcoords);
   game->meshes[0].texcoords = cube_texcoords;
   game->meshes[0].normal_count = countof(cube_normals);
   game->meshes[0].normals = cube_normals;
   game->meshes[0].face_count = countof(cube_faces);
   game->meshes[0].faces = cube_faces;

   // NOTE: Initialize entities.
   for(int index = 0; index < countof(game->entities); ++index)
   {
      entity *e = game->entities + index;
      e->scale = 50.0f * v3(1, 1, 1);

      vec3 origin = {0.5f*backbuffer->width, 0.5f*backbuffer->height, 0};
      e->translation = v3(
         origin.x + 35.0f*(index - countof(game->entities)/2),
         origin.y - 20.0f*(index - countof(game->entities)/2),
         origin.z + 0
      );
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

   // NOTE: Update entities.
   for(int entity_index = 0; entity_index < countof(game->entities); ++entity_index)
   {
      entity *e = game->entities + entity_index;
      mesh_asset mesh = game->meshes[e->mesh_index];

      float turns = 0.1f * dt;
      e->rotation.x += turns;
      e->rotation.y += turns;
      e->rotation.z += turns;

      mat4 world = make_identity();
      world *= make_translation(e->translation.x, e->translation.y, e->translation.z);
      world *= make_rotationx(e->rotation.x);
      world *= make_rotationy(e->rotation.y);
      world *= make_rotationz(e->rotation.z);
      world *= make_scale(e->scale.x, e->scale.y, e->scale.z);

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
