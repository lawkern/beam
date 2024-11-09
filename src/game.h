#pragma once

/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file specifies the API provided by the game. Anything defined here
// is visible to the platform implementations.

#include "shared.h"
#include "memory.h"
#include "math.h"
#include "random.h"
#include "assets.h"
#include "entity.h"
#include "render.h"

#define GAME_TEXTURE_SIZE(t) (sizeof(*((t).memory)) * (t).width * (t).height)

struct game_texture
{
   int width;
   int height;
   u32 *memory;
};

struct game_button
{
   bool pressed;
   bool transitioned;
};


#define GAMECONTROLLER_INDEX_NULL    (-1)
#define GAMECONTROLLER_INDEX_KEYBOARD (0)
#define GAMECONTROLLER_COUNT_MAX      (4)

#define GAME_BUTTONS                            \
   X(action_up)                                 \
   X(action_down)                               \
   X(action_left)                               \
   X(action_right)                              \
   X(move_up)                                   \
   X(move_down)                                 \
   X(move_left)                                 \
   X(move_right)                                \
   X(shoulder_left)                             \
   X(shoulder_right)                            \
   X(start)                                     \
   X(back)

enum game_button_kind
{
#  define X(button_name) GAME_BUTTON_##button_name,
   GAME_BUTTONS
#  undef X

   GAME_BUTTON_COUNT,
};

struct game_controller
{
   union
   {
      struct
      {
#        define X(button_name) game_button button_name;
         GAME_BUTTONS
#        undef X
      };
      game_button buttons[GAME_BUTTON_COUNT];
   };

   bool connected;
};

struct game_input
{
   game_controller controllers[GAMECONTROLLER_COUNT_MAX];
};

struct game_packet
{
   u64 client_id;
   vec3 position;
};

struct server_player
{
   u64 client_id;
   vec3 position;
};

#define SERVERPLAYER_COUNT_MAX 32
struct server_packet
{
   int opponent_count;
   server_player opponents[SERVERPLAYER_COUNT_MAX - GAMECONTROLLER_COUNT_MAX];
};

struct game_context
{
   game_texture backbuffer;

   int input_index;
   game_input inputs[16];

   memarena perma;
   memarena frame;

   random_entropy entropy;
   u64 client_id;

   int triangle_count;
   int triangle_count_max;
   render_triangle *triangles;

   int render_command_count;
   int render_command_count_max;
   render_command *render_commands;

   mat4 view;
   mat4 projection;

   entity entities[256];
   mesh_asset meshes[2];

   bool send_packet;
   game_packet packet;
   server_packet spacket;

   bool running;
};

// NOTE: Initialize any game-specific state. This should be called once at the
// platform-defined entry point.
#define GAME_INITIALIZE(name) void name(game_context *game)

// NOTE: Update the game simulation. This should be called once per frame.
#define GAME_UPDATE(name) void name(game_context *game, float dt)

// NOTE: Render any buffered commands. This should be called once per frame.
#define GAME_RENDER(name) void name(game_context *game)

// NOTE: These expand to forward declarations of the function signatures above,
// in case the macro expansions are confusing. By convention, game functions
// begin with the prefix game_.
GAME_INITIALIZE(game_initialize);
GAME_UPDATE(game_update);
GAME_RENDER(game_render);
