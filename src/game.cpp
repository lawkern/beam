/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "game.h"
#include "platform.h"

#include "memory.cpp"
#include "math.cpp"
#include "assets.cpp"
#include "render.cpp"

static bool is_held(game_button button)
{
   // NOTE(law): The specified button is currently pressed.
   bool result = (button.pressed);
   return(result);
}

static bool was_pressed(game_button button)
{
   // NOTE(law): The specified button was pressed on this frame.
   bool result = (button.pressed && button.transitioned);
   return(result);
}

static bool was_released(game_button button)
{
   // NOTE(law): The specified button was released on this frame.
   bool result = (!button.pressed && button.transitioned);
   return(result);
}

static void print_controller_inputs(game_input *input, int controller_index)
{
   game_controller *con = input->controllers + controller_index;

#  define X(button_name) if(was_pressed(con->buttons[GAME_BUTTON_##button_name])) \
      plog("Controller %d pressed %s\n", controller_index, #button_name);
   GAME_BUTTONS;
#  undef X

#  define X(button_name) if(was_released(con->buttons[GAME_BUTTON_##button_name])) \
      plog("Controller %d released %s\n", controller_index, #button_name);
   GAME_BUTTONS;
#  undef X
}

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
   game->triangle_count_max = 1024 * 1024;
   game->triangles = arena_array(&game->perma, render_triangle, game->triangle_count_max);
   if(!game->triangles)
   {
      plog("ERROR: Failed to allocate the triangle list.\n");
      return;
   }

   game->render_command_count_max = 1024 * 1024;
   game->render_commands = arena_array(&game->perma, render_command, game->render_command_count_max);
   if(!game->render_commands)
   {
      plog("ERROR: Failed to allocate the render command list.\n");
      return;
   }

   float aspect = (float)backbuffer->width / (float)backbuffer->height;
   game->projection = make_perspective(aspect);

   // NOTE: Load pre-bundled assets.
   load_assets(game);

   // NOTE: Initialize entities.
   int entity_index = 0;
   for(int index = 0; index < GAMECONTROLLER_COUNT_MAX; ++index)
   {
      assert(entity_index < countof(game->entities));
      entity *e = game->entities + entity_index++;

      e->facing_direction = v3(1, 0, 0);
      e->scale = v3(0.5, 0.5, 0.5);
      e->translation = v3(0, 0, 0);
      e->mesh_index = 1;
   }

   for(int y = -10; y < 10; ++y)
   {
      for(int x = 2; x < 7; ++x)
      {
         assert(entity_index < countof(game->entities));
         entity *e = game->entities + entity_index++;

         e->translation = v3(5*x, 5*y, 0);
         e->scale = v3(0.5, 0.5, 0.5);
         e->mesh_index = 0;
      }
   }

   // NOTE: Initialization was successful.
   game->running = true;
}

GAME_UPDATE(game_update)
{
   game_texture backbuffer = game->backbuffer;
   game_input *input = game->inputs + game->input_index++;
   game->input_index %= countof(game->inputs);

   memarena *perma = &game->perma;
   memarena *frame = &game->frame;

   // NOTE: Handle user input.
   float delta = dt * 20.0f;

   assert(countof(input->controllers) < countof(game->entities));
   for(int controller_index = 0; controller_index < countof(input->controllers); ++controller_index)
   {
      game_controller *con = input->controllers + controller_index;

      entity *e = game->entities + controller_index;
      if(controller_index == GAMECONTROLLER_INDEX_KEYBOARD || con->connected)
      {
         print_controller_inputs(input, controller_index);

         if(was_pressed(con->back)) game->running = false;

         if(is_held(con->action_down)) e->translation += (e->facing_direction * delta);

         vec3 direction = {0, 0, 0};

         // NOTE: Move forward/back.
         if(is_held(con->move_up))    direction.x += 1;
         if(is_held(con->move_down))  direction.x -= 1;

         // NOTE: Move left/right.
         if(is_held(con->move_left))  direction.y += 1;
         if(is_held(con->move_right)) direction.y -= 1;

         // NOTE: Move up/down.
         // if(is_held(con->shoulder_left))  direction.z += 1;
         // if(is_held(con->shoulder_right)) direction.z -= 1;

         float turns = 0.1f * dt;
         if(is_held(con->shoulder_left))  e->rotation.z -= turns;
         if(is_held(con->shoulder_right)) e->rotation.z -= turns;

         if(direction.x || direction.y || direction.z)
         {
            e->translation += (direction * delta);
            // game->camera_position += (direction * delta);
         }

         if(was_pressed(con->start)) e->translation = v3(0, 0, 0);
      }
   }

   push_clear(game, 0x333333FF);

   // NOTE: Test basic triangle drawing.
   draw_debug_triangles(game);

   entity *player = game->entities + 0;

   vec3 camera_translation = player->translation + v3(-15, 0, 1);
   mat4 view = make_translation(-camera_translation.x, -camera_translation.y, -camera_translation.z);

   // NOTE: Update entities.
   for(int entity_index = 0; entity_index < countof(game->entities); ++entity_index)
   {
      entity *e = game->entities + entity_index;
      mesh_asset mesh = game->meshes[e->mesh_index];

      float turns = 0.1f * dt;
      // e->rotation.x += turns;
      // e->rotation.y += turns;
      // e->rotation.z += turns;

      mat4 scale = make_scale(e->scale.x, e->scale.y, e->scale.z);
      mat4 rotationx = make_rotationx(e->rotation.x);
      mat4 rotationy = make_rotationy(e->rotation.y);
      mat4 rotationz = make_rotationz(e->rotation.z);
      mat4 translation = make_translation(e->translation.x, e->translation.y, e->translation.z);

      mat4 world = translation * scale * rotationx * rotationy * rotationz;

      for(int face_index = 0; face_index < mesh.face_count; ++face_index)
      {
         mesh_asset_face face = mesh.faces[face_index];

         assert(game->triangle_count < game->triangle_count_max);
         int triangle_index = game->triangle_count++;

         render_triangle *triangle = game->triangles + triangle_index;
         triangle->color = (entity_index == 0) ? 0xFF00FFFF : face.color;

         for(int vertex_index = 0; vertex_index < 3; ++vertex_index)
         {
            vec3 vertex = mesh.vertices[face.vertex_indices[vertex_index]];
            vertex *= world;
            vertex *= view;

            // NOTE: Project into clip coordinates.
            vertex = project(game->projection, vertex);

            // NOTE: Convert to screen coordinates.
            vertex.x *= (backbuffer.width / 2.0f);
            vertex.y *= -(backbuffer.height / 2.0f);

            vertex.x += (backbuffer.width / 2.0f);
            vertex.y += (backbuffer.height / 2.0f);

            triangle->vertices[vertex_index] = vertex;
         }

         push_triangle(game, triangle_index);
      }
   }

   // NOTE: End of frame cleanup.
   arena_reset(frame);

   // NOTE: Bulk copy inputs to the next frame.
   game_input *next_input = game->inputs + game->input_index;
   *next_input = *input;

   // NOTE: Clear the transition state for each controller's buttons.
   for(int controller_index = 0; controller_index < countof(input->controllers); ++controller_index)
   {
      game_controller *next = next_input->controllers + controller_index;
      for(int button_index = 0; button_index < GAME_BUTTON_COUNT; ++button_index)
      {
         next->buttons[button_index].transitioned = false;
      }
   }
}

GAME_RENDER(game_render)
{
   game_texture backbuffer = game->backbuffer;

   for(int command_index = 0; command_index < game->render_command_count; ++command_index)
   {
      render_command *command = game->render_commands + command_index;
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
