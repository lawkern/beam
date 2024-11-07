/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "SDL_net.h"
#include "platform_sdl.cpp"
#include "game.h"

int main(int argument_count, char **arguments)
{
   if(SDLNet_Init() == -1)
   {
      plog("ERROR: Failed to initialize SDL netcode. %s\n", SDLNet_GetError());
      return(1);
   }

   UDPsocket sdl_socket = SDLNet_UDP_Open(SERVER_PORT);
   if(!sdl_socket)
   {
      plog("ERROR: Failed to open UDP socket. %s\n", SDLNet_GetError());
      SDLNet_Quit();
      return(1);
   }

   UDPpacket *sdl_packet = SDLNet_AllocPacket(512);
   if(!sdl_packet)
   {
      plog("ERROR: Failed to allocate UDP packet. %s\n", SDLNet_GetError());
      SDLNet_Quit();
      return(1);
   }

   plog("beam server started on %s:%d.\n", SERVER_HOST, SERVER_PORT);

   while(1)
   {
      if(SDLNet_UDP_Recv(sdl_socket, sdl_packet))
      {
         game_packet *packet = (game_packet *)sdl_packet->data;

         vec3 position = packet->position;
         plog("Position: {%0.3f, %0.3f, %0.3f}\n", position.x, position.y, position.z);
      }
   }

   plog("beam server shut down.\n");

   SDLNet_FreePacket(sdl_packet);
   SDLNet_Quit();

   return(0);
}
