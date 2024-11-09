/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "SDL_net.h"

#include "server.h"
#include "platform_sdl.cpp"

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

   UDPpacket *sdl_packet = SDLNet_AllocPacket(SERVER_PACKET_SIZE);
   if(!sdl_packet)
   {
      plog("ERROR: Failed to allocate UDP packet. %s\n", SDLNet_GetError());
      SDLNet_Quit();
      return(1);
   }

   plog("beam server has started on %s:%d.\n", SERVER_HOST, SERVER_PORT);

   server_context server = {};
   while(1)
   {
      if(SDLNet_UDP_Recv(sdl_socket, sdl_packet))
      {
         game_packet *in = (game_packet *)sdl_packet->data;
         server_packet out = server_update(&server, in);

         SDL_memcpy(sdl_packet->data, (void *)&out, sizeof(out));
         sdl_packet->len = sizeof(out);

         SDLNet_UDP_Send(sdl_socket, -1, sdl_packet);
      }
   }

   plog("beam server has shut down.\n");

   SDLNet_FreePacket(sdl_packet);
   SDLNet_Quit();

   return(0);
}
