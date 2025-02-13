/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file is the entry point to the SDL-based port. Platform-specific
// functionality should be relegated to the platform_*.cpp implementation files,
// as specified by platform.h. This file should compile on all platforms that
// support SDL. It may be superceded by dedicated main_*.cpp files in the
// future, e.g. main_win32.cpp, main_macos.cpp, etc.

#include "game.h"
#include "platform.h"

int main(int argument_count, char **arguments)
{
   game_context game = {};
   game_initialize(&game);

   platform_initialize(game.backbuffer.width, game.backbuffer.height);

   while(platform_frame_begin(game.inputs + game.input_index))
   {
      game_update(&game);
      game_render(&game);

      platform_render(game.backbuffer);
      platform_frame_end(&game);
   }

   return(0);
}
