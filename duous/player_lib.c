#include "player_lib.h"
#include "game.h"
#include "server.h"

int CHOST_Player_get_username(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -1);
	int i;
	lua_pop(L, 1);
	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_pushstring(L, player->username);
		return 1;
	}
	else
	{
		return 0;
	}
}
int CHOST_Player_set_username(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -2);
	int i;

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		const char *username = lua_tostring(L, -1);
		player_set_username(player, username);
	}
	lua_pop(L, 2);
	return 0;
}
int CHOST_Player_get_client(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -1);
	int i;

	lua_pop(L, 1);

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_push_client_object(L, player->client);
		return 1;
	}
	else
	{
		return 0;
	}
}
int CHOST_Player_get_position(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -1);
	int i;

	lua_pop(L, 1);

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_push_position_object(L, player->pos);
		return 1;
	}
	else
	{
		return 0;
	}
}
int CHOST_Player_set_position(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -2);
	int i;

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_pushstring(L, "x");
		lua_gettable(L, -2);
		player->pos.x = lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pushstring(L, "y");
		lua_gettable(L, -2);
		player->pos.y = lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pushstring(L, "z");
		lua_gettable(L, -2);
		player->pos.z = lua_tonumber(L, -1);
		lua_pop(L, 1);

	}
	lua_pop(L, 2);
	return 0;
}
int CHOST_Player_get_look(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -1);
	int i;

	lua_pop(L, 1);
	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_push_look_object(L, player->look);
		return 1;
	}
	else
	{
		return 0;
	}
}
int CHOST_Player_set_look(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -2);
	int i;

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_pushstring(L, "yaw");
		lua_gettable(L, -2);
		player->look.yaw = lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pushstring(L, "pitch");
		lua_gettable(L, -2);
		player->look.pitch = lua_tonumber(L, -1);
		lua_pop(L, 1);

	}
	lua_pop(L, 2);
	return 0;
}
int CHOST_Player_get_on_ground(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -1);
	int i;
	lua_pop(L, 1);

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_pushboolean(L, player->on_ground);
		return 1;
	}
	else
	{
		return 0;
	}
}
int CHOST_Player_set_on_ground(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -2);
	int i;

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		player->on_ground = lua_toboolean(L, -1);
	}
	lua_pop(L, 2);
	return 0;

}
int CHOST_Player_get_current_slot(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -1);
	int i;
	lua_pop(L, 1);
	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		lua_pushnumber(L, player->current_slot);
		return 1;
	}
	else
	{
		return 0;
	}
}
int CHOST_Player_set_current_slot(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct player *player = NULL;
	bool found_player = false;
	int player_id = lua_tonumber(L, -2);
	int i;

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->id == player_id)
		{
			found_player = true;
			player = game->players[i];
		}
	}

	if(found_player)
	{
		player->current_slot = lua_tonumber(L, -1);
	}
	lua_pop(L, 2);
	return 0;
}
