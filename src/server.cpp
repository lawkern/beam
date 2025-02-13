/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "server.h"

SERVER_UPDATE(server_update)
{
   server_packet result = {};

   // NOTE: Find the player that corresponds to the client who sent the packet.
   server_player *player = 0;
   for(int player_index = 0; player_index < SERVERPLAYER_COUNT_MAX; ++player_index)
   {
      server_player *test = server->players + player_index;
      if(test->client_id == packet->client_id)
      {
         player = test;
         break;
      }
   }

   // NOTE: If this player is not registered in the server yet, add them.
   if(!player)
   {
      assert(server->player_count < SERVERPLAYER_COUNT_MAX);

      player = server->players + server->player_count++;
      player->client_id = packet->client_id;

      platform_log("Client %llu joined the server (player count = %d).\n", player->client_id, server->player_count);
   }

   // NOTE: Update the player's position on the server.
   player->position = packet->position;

   platform_log("pos (%x): {%0.3f, %0.3f, %0.3f}\n",
                player->client_id,
                player->position.x,
                player->position.y,
                player->position.z);

   // NOTE: Send back information on all other player locations.
   result.opponent_count = 0;
   for(int player_index = 0; player_index < SERVERPLAYER_COUNT_MAX; ++player_index)
   {
      server_player *opponent = server->players + player_index;
      if(opponent->client_id && opponent->client_id != player->client_id)
      {
         result.opponents[player_index] = *opponent;
         result.opponent_count++;
      }
   }

   return(result);
}
