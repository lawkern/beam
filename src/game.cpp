/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "game.h"
#include "platform.h"

#include "memory.cpp"
#include "math.cpp"
#include "random.cpp"
#include "assets.cpp"
#include "render.cpp"
#include "entity.cpp"

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
   // NOTE: Initialize random entropy.
   game->entropy = random_seed((u64)game);
   game->client_id = random_value(&game->entropy);

   // TODO: This is not a particularly smart way to differentiate clients. Using
   // the game_context pointer as a seed value is also a bit silly.
   plog("Client ID: %llu\n", game->client_id);

   // NOTE: Initialize memory.
   game->perma = arena_new(MEGABYTES(512 * 4));
   game->frame = arena_new(MEGABYTES(128));
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
   game->triangle_count_max = 1024 * 1024 * 8;
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

   float aspectx = (float)backbuffer->width / (float)backbuffer->height;
   float aspecty = (float)backbuffer->height / (float)backbuffer->width;

   float fov = 1.0f / 6.0f; // PI/3
   float near = 0.1f;
   float far = 100.0f;

   game->projection = make_perspective(aspectx, near, far);
   initialize_frustum_planes(aspectx, fov, near, far);

   // NOTE: Load pre-bundled assets.
   load_assets(game);

   // NOTE: Initialize entities.
   initialize_entities(game);

   // NOTE: Initialization was successful.
   game->running = true;
}

GAME_UPDATE(game_update)
{
   // NOTE: Set up the frame.
   game_texture backbuffer = game->backbuffer;
   game_input *input = game->inputs + game->input_index++;
   game->input_index %= countof(game->inputs);

   memarena *perma = &game->perma;
   memarena *frame = &game->frame;

   push_clear(game, 0x333333FF);

   // NOTE: Handle user input.
   float delta = dt * 20.0f;

   assert(countof(input->controllers) < countof(game->entities));
   for(int controller_index = 0; controller_index < countof(input->controllers); ++controller_index)
   {
      game_controller *con = input->controllers + controller_index;

      entity *e = game->entities + controller_index;
      if(controller_index == GAMECONTROLLER_INDEX_KEYBOARD || con->connected)
      {
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
            e->active = true;
            e->translation += (direction * delta);
            // game->camera_position += (direction * delta);
         }

         if(was_pressed(con->start)) game->send_packet = !game->send_packet;
      }
   }

   // NOTE: Test basic triangle drawing.
   draw_debug_triangles(game);

   entity *player = game->entities + 0;
   vec3 camera_translation = player->translation + v3(-15, 0, 1);
   game->view = make_translation(-camera_translation.x, -camera_translation.y, -camera_translation.z);

   // NOTE: Update entities.
   if(game->send_packet)
   {
      for(int index = 0; index < SERVERPLAYER_COUNT_MAX; ++index)
      {
         server_player *opponent = game->spacket.opponents + index;
         if(opponent->client_id != game->client_id)
         {
            // TODO: Stop relying on hard-coded offsets to determine the type of
            // entity we're looking at.
            entity *opponent_entity = game->entities + GAMECONTROLLER_COUNT_MAX + index;
            opponent_entity->active = true;
            opponent_entity->translation = opponent->position;
         }
      }
   }

   for(int entity_index = 0; entity_index < countof(game->entities); ++entity_index)
   {
      update_entity(game, entity_index, backbuffer);
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

   // NOTE: Store data to be delivered to server.
   if(game->send_packet)
   {
      game->packet.client_id = game->client_id;
      game->packet.position = player->translation;
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

   vec2i v0 = {10, 10};
   vec2i v1 = {100, 100};
   vec2i v2 = {10, 100};
   vec2i v3 = {100, 10};

   draw_filled_triangle(backbuffer, v0, v1, v2, 0xFFFFFFFF);
   draw_filled_triangle(backbuffer, v0, v3, v1, 0x55FFFFFF);

   // NOTE: Clear this frame's renderer state.
   game->render_command_count = 0;
   game->triangle_count = 0;
}
