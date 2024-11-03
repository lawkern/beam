/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

// NOTE: This file is the entry point to the SDL-based port. Platform-specific
// functionality should be relegated to the platform_*.cpp implementation files,
// as specified by platform.h. This file should compile on all platforms that
// support SDL. It may be superceded by dedicated main_*.cpp files in the
// future, e.g. main_win32.cpp, main_macos.cpp, etc.

// NOTE: The SDL header is included using quotes instead of angle brackets to
// ensure we get the version SDL2-config picks up, instead of whatever is lying
// around in PATH.
#include "SDL.h"

#include "game.h"
#include "platform.h"

struct sdl_context
{
   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *texture;
   SDL_GameController *controllers[GAME_CONTROLLER_COUNT_MAX];
   SDL_DisplayMode display_mode;

   bool running;
   u64 frequency;
   u64 frame_start;
   u64 frame_count;

   int refresh_rate;
   float target_frame_seconds;
   float actual_frame_seconds;
};

PLATFORM_LOG(plog)
{
   va_list arguments;
   va_start(arguments, fmt);
   {
      SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, arguments);
   }
   va_end(arguments);
}

PLATFORM_ALLOCATE(pallocate)
{
   return SDL_calloc(1, size);
}

PLATFORM_DEALLOCATE(pdeallocate)
{
   SDL_free(memory);
}

static void sdl_initialize(sdl_context *sdl, int width, int height)
{
   SDL_Init(SDL_INIT_EVERYTHING);

   sdl->window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
   if(!sdl->window)
   {
      plog("ERROR: Failed to create SDL window.\n");
      return;
   }

   sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_PRESENTVSYNC);
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

static void sdl_connect_controller(sdl_context *sdl, game_input *input)
{
   // NOTE: Find the first available controller slot and store the controller
   // pointer. The indices for sdl->controllers and input->controllers must be
   // manually maintained.

   for(int controller_index = 0; controller_index < GAME_CONTROLLER_COUNT_MAX; ++controller_index)
   {
      if(sdl->controllers[controller_index] == 0 && SDL_IsGameController(controller_index))
      {
         sdl->controllers[controller_index] = SDL_GameControllerOpen(controller_index);
         if(sdl->controllers[controller_index])
         {
            input->controllers[controller_index].connected = true;
            plog("Controller added at slot %d\n", controller_index);
         }
         else
         {
            plog("ERROR: %s\n", SDL_GetError());
         }

         // TODO: Should we only break on success? Look into what types of
         // errors are actually caught here.
         break;
      }
   }
}

static void sdl_disconnect_controller(sdl_context *sdl, game_input *input, int controller_index)
{
   assert(controller_index >= 0);
   assert(controller_index < GAME_CONTROLLER_COUNT_MAX);
   assert(sdl->controllers[controller_index]);

   SDL_GameControllerClose(sdl->controllers[controller_index]);

   sdl->controllers[controller_index] = 0;
   input->controllers[controller_index].connected = false;

   plog("Controller removed from slot %d\n", controller_index);
}

static int sdl_get_controller_index(sdl_context *sdl, SDL_JoystickID id)
{
   int result = GAME_CONTROLLER_INDEX_NULL;

   for(int controller_index = 0; controller_index < GAME_CONTROLLER_COUNT_MAX; ++controller_index)
   {
      SDL_GameController *test = sdl->controllers[controller_index];
      if(test)
      {
         SDL_Joystick *joystick = SDL_GameControllerGetJoystick(test);
         if(id == SDL_JoystickInstanceID(joystick))
         {
            result = controller_index;
            break;
         }
      }
   }

   return(result);
}

static void sdl_process_button(game_button *button, bool pressed)
{
   button->pressed = pressed;
   button->transitioned = true;
}

static void sdl_process_input(sdl_context *sdl, game_input *input)
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
            game_controller *keyboard = input->controllers + GAME_CONTROLLER_INDEX_KEYBOARD;

            bool pressed = event.type == SDL_KEYDOWN;
            switch(event.key.keysym.sym)
            {
               case SDLK_ESCAPE: {sdl->running = false;} break;
               case SDLK_f: {
                  if(pressed)
                  {
                     bool is_fullscreen = (SDL_GetWindowFlags(sdl->window) & SDL_WINDOW_FULLSCREEN_DESKTOP);
                     SDL_SetWindowFullscreen(sdl->window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                  }
               } break;

               case SDLK_i: {sdl_process_button(&keyboard->action_up, pressed);} break;
               case SDLK_k: {sdl_process_button(&keyboard->action_down, pressed);} break;
               case SDLK_j: {sdl_process_button(&keyboard->action_left, pressed);} break;
               case SDLK_l: {sdl_process_button(&keyboard->action_right, pressed);} break;

               case SDLK_w: {sdl_process_button(&keyboard->move_up, pressed);} break;
               case SDLK_s: {sdl_process_button(&keyboard->move_down, pressed);} break;
               case SDLK_a: {sdl_process_button(&keyboard->move_left, pressed);} break;
               case SDLK_d: {sdl_process_button(&keyboard->move_right, pressed);} break;

               case SDLK_q: {sdl_process_button(&keyboard->shoulder_left, pressed);} break;
               case SDLK_o: {sdl_process_button(&keyboard->shoulder_right, pressed);} break;
               case SDLK_SPACE:     {sdl_process_button(&keyboard->start, pressed);} break;
               case SDLK_BACKSPACE: {sdl_process_button(&keyboard->back, pressed);} break;
            }
         } break;

         case SDL_CONTROLLERBUTTONUP:
         case SDL_CONTROLLERBUTTONDOWN: {
            int controller_index = sdl_get_controller_index(sdl, event.cdevice.which);
            if(controller_index != GAME_CONTROLLER_INDEX_NULL)
            {
               game_controller *con = input->controllers + controller_index;

               bool pressed = event.cbutton.state == SDL_PRESSED;
               switch(event.cbutton.button)
               {
                  // TODO: Confirm if other controllers map buttons based on name or position.
                  case SDL_CONTROLLER_BUTTON_A: {sdl_process_button(&con->action_down, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_B: {sdl_process_button(&con->action_right, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_X: {sdl_process_button(&con->action_left, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_Y: {sdl_process_button(&con->action_up, pressed);} break;

                  case SDL_CONTROLLER_BUTTON_DPAD_UP:    {sdl_process_button(&con->move_up, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  {sdl_process_button(&con->move_down, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:  {sdl_process_button(&con->move_left, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: {sdl_process_button(&con->move_right, pressed);} break;

                  case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  {sdl_process_button(&con->shoulder_left, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: {sdl_process_button(&con->shoulder_right, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_START: {sdl_process_button(&con->start, pressed);} break;
                  case SDL_CONTROLLER_BUTTON_BACK:  {sdl_process_button(&con->back, pressed);} break;
               }
            }
         } break;

         case SDL_CONTROLLERDEVICEADDED: {
            sdl_connect_controller(sdl, input);
         } break;

         case SDL_CONTROLLERDEVICEREMOVED: {
            int controller_index = sdl_get_controller_index(sdl, event.cdevice.which);
            if(controller_index != GAME_CONTROLLER_INDEX_NULL)
            {
               sdl_disconnect_controller(sdl, input, controller_index);
            }
         } break;
      }
   }
}

static void sdl_render(sdl_context *sdl, game_texture backbuffer)
{
   // NOTE: Clear the background to black, so that black bars are displayed when
   // the aspect ratio of the backbuffer and window don't match.
   SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);
   SDL_RenderClear(sdl->renderer);

   // NOTE: Compute the destination size of the displayed backbuffer, accounting
   // for aspect ratio differents.
   int src_width = backbuffer.width;
   int src_height = backbuffer.height;

   int dst_width, dst_height;
   SDL_GetRendererOutputSize(sdl->renderer, &dst_width, &dst_height);

   float src_aspect = (float)src_width / (float)src_height;
   float dst_aspect = (float)dst_width / (float)dst_height;

   SDL_Rect dst_rect = {0, 0, dst_width, dst_height};
   if(src_aspect > dst_aspect)
   {
      // NOTE: Bars on top and bottom.
      int bar_height = (int)(0.5f * (dst_height - (dst_width / src_aspect)));
      dst_rect.y += bar_height;
      dst_rect.h -= (bar_height * 2);
   }
   else if(src_aspect < dst_aspect)
   {
      // NOTE: Bars on left and right;
      int bar_width = (int)(0.5f * (dst_width - (dst_height * src_aspect)));
      dst_rect.x += bar_width;
      dst_rect.w -= (bar_width * 2);
   }

   // NOTE: Copy the backbuffer to SDL's renderer and present.
   SDL_UpdateTexture(sdl->texture, 0, backbuffer.memory, backbuffer.width * sizeof(*backbuffer.memory));
   SDL_RenderCopy(sdl->renderer, sdl->texture, 0, &dst_rect);

   SDL_RenderPresent(sdl->renderer);
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
   sdl->frame_count++;

#if DEBUG
   if((sdl->frame_count % sdl->refresh_rate) == 0)
   {
      float frame_ms = sdl->actual_frame_seconds * 1000.0f;
      plog("Frame time: % .3fms (Work: % .3fms, Sleep: % 3dms)\n", frame_ms, work_ms, sleep_ms);
   }
#endif
}

int main(int argument_count, char **arguments)
{
   game_context game = {};
   game_initialize(&game);

   sdl_context sdl = {};
   sdl_initialize(&sdl, game.backbuffer.width, game.backbuffer.height);

   while(game.running && sdl.running)
   {
      sdl_process_input(&sdl, game.inputs + game.input_index);

      game_update(&game, sdl.actual_frame_seconds);
      game_render(&game);

      sdl_render(&sdl, game.backbuffer);
      sdl_frame_end(&sdl);
   }

   return(0);
}
