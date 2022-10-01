#include "game_lib.h"
#include "game.h"
#include "server.h"

int CHOST_Game_get_max_players(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	lua_pushnumber(L, game->max_players);
	return 1;
}
int CHOST_Game_set_max_players(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	int max = lua_tonumber(L, -1);
	lua_pop(L, 1);
	game->max_players = max;
	return 0;
}
int CHOST_Game_get_num_players(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	lua_pushnumber(L, game->num_players);
	return 1;
}
int CHOST_Game_add_player(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct client *client = NULL;
	bool found_client = false;
	int client_id = lua_tonumber(L, -1);
	int i;
	lua_pop(L, 1);

	for(i = 0; i < server->num_clients; i++)
	{
		client = server->clients[i];
		if(client->id == client_id)
		{
			found_client = true;
			break;
		}
	}

	if(found_client)
	{
		game_new_player(server->game, client);
		return 1;
	}
	else
	{
		return 0;
	}
}
