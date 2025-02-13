/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "SDL3/SDL.h"
#include "platform.h"

PLATFORM_LOG(platform_log)
{
   va_list arguments;
   va_start(arguments, fmt);
   {
      SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, arguments);
   }
   va_end(arguments);
}

PLATFORM_ALLOCATE(platform_allocate)
{
   return SDL_calloc(1, size);
}

PLATFORM_DEALLOCATE(platform_deallocate)
{
   SDL_free(memory);
}

static struct {
   SDL_Window *window;
   SDL_Renderer *renderer;
   SDL_Texture *texture;
   SDL_Gamepad *controllers[GAMECONTROLLER_COUNT_MAX];
   SDL_DisplayMode display_mode;

#if NETWORKING_SUPPORTED
   UDPsocket socket;
   IPaddress server_address;
   UDPpacket *packet;
#endif

   u64 frequency;
   u64 frame_start;
   u64 frame_count;

   int refresh_rate;
   float target_frame_seconds;
   float actual_frame_seconds;
} sdl;

PLATFORM_INITIALIZE(platform_initialize)
{
   if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMEPAD))
   {
      platform_log("ERROR: Failed to initialize SDL3.");
      assert(0);
   }

   if(!SDL_CreateWindowAndRenderer(TITLE, width, height, 0, &sdl.window, &sdl.renderer))
   {
      platform_log("ERROR: Failed to create window/renderer.");
      assert(0);
   }

   if(!SDL_SetRenderVSync(sdl.renderer, 1))
   {
      SDL_Log("WARNING: Failed to set vsync.");
   }

   sdl.texture = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
   if(!sdl.texture)
   {
      platform_log("ERROR: Failed to create SDL texture.\n");
      assert(0);
   }

   // TODO: Handle multiple monitors properly.
   // sdl.display_mode = SDL_GetDesktopDisplayMode(0);
   // if(!sdl.display_mode)
   // {
   //    platform_log("ERROR: Failed to query desktop display mode.\n");
   //    assert(0);
   // }

   // NOTE: Initialize frame information.
   sdl.frequency = SDL_GetPerformanceFrequency();

   sdl.refresh_rate = 60;
   // if(sdl.display_mode.refresh_rate > 0)
   // {
   //    sdl.refresh_rate = sdl.display_mode.refresh_rate;
   // }

   sdl.target_frame_seconds = 1.0f / sdl.refresh_rate;

   platform_log("Monitor refresh rate: %d\n", sdl.refresh_rate);
   platform_log("Target frame time: %0.03fms\n", sdl.target_frame_seconds * 1000.0f);

#if NETWORKING_SUPPORTED
   // NOTE: Initialize netcode.
   if(SDLNet_Init() == -1)
   {
      platform_log("ERROR: Failed to initialize SDL netcode. %s\n", SDLNet_GetError());
      assert(0);
   }

   sdl.socket = SDLNet_UDP_Open(0);
   if(!sdl.socket)
   {
      platform_log("ERROR: Failed to open UDP socket. %s\n", SDLNet_GetError());
      SDLNet_Quit();
      assert(0);
   }

   if(SDLNet_ResolveHost(&sdl.server_address, SERVER_HOST, SERVER_PORT))
   {
      platform_log("ERROR: Failed to resolve host (%s %d): %s\n", SERVER_HOST, SERVER_PORT, SDLNet_GetError());
      SDLNet_Quit();
      assert(0);
   }

   sdl.packet = SDLNet_AllocPacket(512);
   if(!sdl.packet)
   {
      platform_log("ERROR: Failed to allocate UDP packet. %s\n", SDLNet_GetError());
      SDLNet_Quit();
      assert(0);
   }
#endif
}

static void sdl_connect_controller(game_input *input)
{
   // NOTE: Find the first available controller slot and store the controller
   // pointer. The indices for sdl.controllers and input->controllers must be
   // manually maintained.

   for(int controller_index = 0; controller_index < GAMECONTROLLER_COUNT_MAX; ++controller_index)
   {
      if(sdl.controllers[controller_index] == 0 && SDL_IsGamepad(controller_index))
      {
         sdl.controllers[controller_index] = SDL_OpenGamepad(controller_index);
         if(sdl.controllers[controller_index])
         {
            input->controllers[controller_index].connected = true;
            platform_log("Controller added at slot %d\n", controller_index);
         }
         else
         {
            platform_log("ERROR: %s\n", SDL_GetError());
         }

         // TODO: Should we only break on success? Look into what types of
         // errors are actually caught here.
         break;
      }
   }
}

static void sdl_disconnect_controller(game_input *input, int controller_index)
{
   assert(controller_index >= 0);
   assert(controller_index < GAMECONTROLLER_COUNT_MAX);
   assert(sdl.controllers[controller_index]);

   SDL_CloseGamepad(sdl.controllers[controller_index]);

   sdl.controllers[controller_index] = 0;
   input->controllers[controller_index].connected = false;

   platform_log("Controller removed from slot %d\n", controller_index);
}

static int sdl_get_controller_index(SDL_JoystickID id)
{
   int result = GAMECONTROLLER_INDEX_NULL;

   for(int controller_index = 0; controller_index < GAMECONTROLLER_COUNT_MAX; ++controller_index)
   {
      SDL_Gamepad *test = sdl.controllers[controller_index];
      if(test)
      {
         SDL_Joystick *joystick = SDL_GetGamepadJoystick(test);
         if(id == SDL_GetJoystickID(joystick))
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

PLATFORM_FRAME_BEGIN(platform_frame_begin)
{
   bool keep_running = true;

   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      switch(event.type)
      {
         case SDL_EVENT_QUIT: {
            keep_running = false;
         } break;

         case SDL_EVENT_KEY_UP:
         case SDL_EVENT_KEY_DOWN: {
            game_controller *keyboard = input->controllers + GAMECONTROLLER_INDEX_KEYBOARD;

            bool pressed = event.key.down;
            switch(event.key.key)
            {
               case SDLK_ESCAPE: {keep_running = false;} break;
               case SDLK_F: {
                  if(pressed)
                  {
                     bool is_fullscreen = (SDL_GetWindowFlags(sdl.window) & SDL_WINDOW_FULLSCREEN);
                     SDL_SetWindowFullscreen(sdl.window, is_fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                  }
               } break;

               case SDLK_I: {sdl_process_button(&keyboard->action_up, pressed);} break;
               case SDLK_K: {sdl_process_button(&keyboard->action_down, pressed);} break;
               case SDLK_J: {sdl_process_button(&keyboard->action_left, pressed);} break;
               case SDLK_L: {sdl_process_button(&keyboard->action_right, pressed);} break;

               case SDLK_W: {sdl_process_button(&keyboard->move_up, pressed);} break;
               case SDLK_S: {sdl_process_button(&keyboard->move_down, pressed);} break;
               case SDLK_A: {sdl_process_button(&keyboard->move_left, pressed);} break;
               case SDLK_D: {sdl_process_button(&keyboard->move_right, pressed);} break;

               case SDLK_Q: {sdl_process_button(&keyboard->shoulder_left, pressed);} break;
               case SDLK_E: {sdl_process_button(&keyboard->shoulder_right, pressed);} break;
               case SDLK_SPACE:     {sdl_process_button(&keyboard->start, pressed);} break;
               case SDLK_BACKSPACE: {sdl_process_button(&keyboard->back, pressed);} break;
            }
         } break;

         case SDL_EVENT_GAMEPAD_BUTTON_UP:
         case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
            int controller_index = sdl_get_controller_index(event.cdevice.which);
            if(controller_index != GAMECONTROLLER_INDEX_NULL)
            {
               game_controller *con = input->controllers + controller_index;

               bool pressed = event.button.down;
               switch(event.button.button)
               {
                  // TODO: Confirm if other controllers map buttons based on name or position.
                  case SDL_GAMEPAD_BUTTON_SOUTH: {sdl_process_button(&con->action_down, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_EAST:  {sdl_process_button(&con->action_right, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_WEST:  {sdl_process_button(&con->action_left, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_NORTH: {sdl_process_button(&con->action_up, pressed);} break;

                  case SDL_GAMEPAD_BUTTON_DPAD_UP:    {sdl_process_button(&con->move_up, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_DPAD_DOWN:  {sdl_process_button(&con->move_down, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_DPAD_LEFT:  {sdl_process_button(&con->move_left, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: {sdl_process_button(&con->move_right, pressed);} break;

                  case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:  {sdl_process_button(&con->shoulder_left, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER: {sdl_process_button(&con->shoulder_right, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_START: {sdl_process_button(&con->start, pressed);} break;
                  case SDL_GAMEPAD_BUTTON_BACK:  {sdl_process_button(&con->back, pressed);} break;
               }
            }
         } break;

         case SDL_EVENT_GAMEPAD_ADDED: {
            sdl_connect_controller(input);
         } break;

         case SDL_EVENT_GAMEPAD_REMOVED: {
            int controller_index = sdl_get_controller_index(event.cdevice.which);
            if(controller_index != GAMECONTROLLER_INDEX_NULL)
            {
               sdl_disconnect_controller(input, controller_index);
            }
         } break;
      }
   }

   return(keep_running);
}

PLATFORM_RENDER(platform_render)
{
   // NOTE: Clear the background to black, so that black bars are displayed when
   // the aspect ratio of the backbuffer and window don't match.
   SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 255);
   SDL_RenderClear(sdl.renderer);

   // NOTE: Compute the destination size of the displayed backbuffer, accounting
   // for aspect ratio differents.
   int src_width = backbuffer.width;
   int src_height = backbuffer.height;

   int dst_width, dst_height;
   SDL_GetCurrentRenderOutputSize(sdl.renderer, &dst_width, &dst_height);

   float src_aspect = (float)src_width / (float)src_height;
   float dst_aspect = (float)dst_width / (float)dst_height;

   SDL_FRect dst_rect = {0, 0, (float)dst_width, (float)dst_height};
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
   SDL_UpdateTexture(sdl.texture, 0, backbuffer.memory, backbuffer.width * sizeof(*backbuffer.memory));
   SDL_RenderTexture(sdl.renderer, sdl.texture, 0, &dst_rect);

   SDL_RenderPresent(sdl.renderer);
}

#define ELAPSED_SECONDS(start, end, freq) ((float)((end) - (start)) / (float)(freq))

#if NETWORKING_SUPPORTED
static void sdl_exchange_packets(game_context *game)
{
   memsize packet_size = sizeof(game->packet);
   SDL_memcpy(sdl.packet->data, (void *)&game->packet, packet_size);

   sdl.packet->len = packet_size;
   sdl.packet->address.host = sdl.server_address.host;
   sdl.packet->address.port = sdl.server_address.port;

   SDLNet_UDP_Send(sdl.socket, -1, sdl.packet);

   int packet_count = SDLNet_UDP_Recv(sdl.socket, sdl.packet);
   if(packet_count == -1)
   {
      platform_log("ERROR: Client failed to receive packet. %s\n", SDLNet_GetError());
   }
   else if(packet_count == 1)
   {
      game->spacket = *(server_packet *)sdl.packet->data;
   }
}
#endif

PLATFORM_FRAME_END(platform_frame_end)
{
#if NETWORKING_SUPPORTED
   // NOTE: Send this frame's game data to the server and store any response we
   // get back.
   if(game->send_packet)
   {
      sdl_exchange_packets(sdl, game);
   }
#endif

   // NOTE: Compute how much time has elapsed this frame.
   float target = sdl.target_frame_seconds;
   u64 freq = sdl.frequency;
   u64 start = sdl.frame_start;
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
   sdl.frame_start = SDL_GetPerformanceCounter();
   sdl.actual_frame_seconds = dt;
   sdl.frame_count++;

#if DEBUG
   if((sdl.frame_count % sdl.refresh_rate) == 0)
   {
      // float frame_ms = sdl.actual_frame_seconds * 1000.0f;
      // platform_log("Frame time: % .3fms (Worked: % .3fms, Requested sleep: % 3dms)\n", frame_ms, work_ms, sleep_ms);
   }
#endif
}
