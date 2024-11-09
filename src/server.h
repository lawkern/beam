/* /////////////////////////////////////////////////////////////////////////// */
/* (c) copyright 2024 Lawrence D. Kern /////////////////////////////////////// */
/* /////////////////////////////////////////////////////////////////////////// */

#include "game.h"
#include "platform.h"

struct server_context
{
   int player_count;
   server_player players[SERVERPLAYER_COUNT_MAX];
};

#define SERVER_UPDATE(name) server_packet name(server_context *server, game_packet *packet)
SERVER_UPDATE(server_update);
