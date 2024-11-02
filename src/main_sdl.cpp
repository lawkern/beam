/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file is the entry point to the SDL-based port. Platform-specific
// functionality should be relegated to the platform_*.cpp implementation files,
// as specified by platform.h. This file should compile on all platforms that
// support SDL. It may be superceded by dedicated main_*.cpp files in the
// future, e.g. main_win32.cpp, main_macos.cpp, etc.

#include <SDL.h>

#include "platform.h"

struct sdl_context
{
   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *texture;
   SDL_GameController *gamepad;
   SDL_DisplayMode display_mode;

   bool running;
   u64 frequency;
   u64 frame_start;
   u64 frame_count;

   int refresh_rate;
   float target_frame_seconds;
   float actual_frame_seconds;
};

static void sdl_initialize(sdl_context *sdl, int width, int height)
{
   SDL_Init(SDL_INIT_EVERYTHING);

   sdl->window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_ALLOW_HIGHDPI);
   if(!sdl->window)
   {
      plog("ERROR: Failed to create SDL window.\n");
      return;
   }

   sdl->renderer = SDL_CreateRenderer(sdl->window, -1, 0);
   if(!sdl->renderer)
   {
      plog("ERROR: Failed to create SDL renderer.\n");
      return;
   }

   sdl->texture = SDL_CreateTexture(sdl->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
   if(!sdl->texture)
   {
      plog("ERROR: Failed to create SDL texture.\n");
      return;
   }

   // TODO: Handle multiple monitors properly.
   if(SDL_GetDesktopDisplayMode(0, &sdl->display_mode))
   {
      plog("ERROR: Failed to query desktop display mode.\n");
      return;
   }

   // NOTE: Initialize frame information.
   sdl->frequency = SDL_GetPerformanceFrequency();

   sdl->refresh_rate = 60;
   if(sdl->display_mode.refresh_rate > 0)
   {
      sdl->refresh_rate = sdl->display_mode.refresh_rate;
   }

   sdl->target_frame_seconds = 1.0f / sdl->refresh_rate;

   plog("Monitor refresh rate: %d\n", sdl->refresh_rate);
   plog("Target frame time: %0.03fms\n", sdl->target_frame_seconds * 1000.0f);

   // NOTE: Initialization succeeded, main loop can begin.
   sdl->running = true;
}

static void sdl_process_input(sdl_context *sdl)
{
   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
         case SDL_QUIT: {
            sdl->running = false;
         } break;

         case SDL_KEYUP:
         case SDL_KEYDOWN: {
            bool pressed = event.type == SDL_KEYDOWN;
            switch(event.key.keysym.sym)
            {
               case SDLK_ESCAPE: {sdl->running = false;} break;
            }
         } break;
      }
   }
}

#define ELAPSED_SECONDS(start, end, freq) ((float)((end) - (start)) / (float)(freq))

static void sdl_frame_end(sdl_context *sdl)
{
   // NOTE: Compute how much time has elapsed this frame.
   float target = sdl->target_frame_seconds;
   u64 freq = sdl->frequency;
   u64 start = sdl->frame_start;
   u64 end = SDL_GetPerformanceCounter();

   float dt = ELAPSED_SECONDS(start, end, freq);
   float work_ms = dt * 1000.0f;
   int sleep_ms = 0;

   // NOTE: Sleep through excess frame time to enforce frame rate.
   if(dt < target)
   {
      sleep_ms = (int)((target - dt) * 1000.0f) - 1;
      if(sleep_ms > 0)
      {
         SDL_Delay(sleep_ms);
      }
   }

   // NOTE: Spinlock for the remaining frame time, if any remains.
   while(dt < target)
   {
      end = SDL_GetPerformanceCounter();
      dt = ELAPSED_SECONDS(start, end, freq);
   }

   // NOTE: Update values for next frame.
   sdl->frame_start = SDL_GetPerformanceCounter();
   sdl->actual_frame_seconds = dt;

#if DEBUG
   if((sdl->frame_count % sdl->refresh_rate) == 0)
   {
      plog("Actual frame time: %0.3fms\n", sdl->actual_frame_seconds * 1000.0f);
      plog("   Work: %0.3fms\n", work_ms);
      plog("   Sleep: %dms\n", sleep_ms);
      plog("---------------------------\n");
   }
#endif
   sdl->frame_count++;
}


int main(int argument_count, char **arguments)
{
   sdl_context sdl = {};
   sdl_initialize(&sdl, 1920 / 2, 1080 / 2);

   while(sdl.running)
   {
      sdl_process_input(&sdl);

      // TODO: Update and render.

      sdl_frame_end(&sdl);
   }

   return(0);
}
