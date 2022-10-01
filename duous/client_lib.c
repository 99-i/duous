#include "client_lib.h"
#include "game.h"
#include "server.h"

int CHOST_Client_get_state(lua_State *L)
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
		lua_pushstring(L, client_state_to_cstr(client->state));
		return 1;
	}
	else
	{
		return 0;
	}



}
int CHOST_Client_set_state(lua_State *L)
{
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct client *client = NULL;
	bool found_client = false;
	const char *state_str = lua_tostring(L, -1);
	int client_id = lua_tonumber(L, -2);
	int i;
	client_state desired_state;
	lua_pop(L, -2);

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
		desired_state = cstr_to_client_state(state_str);
		if(desired_state != -1)
		{
			client->state = desired_state;
		}
		return 0;
	}
	else
	{
		return 0;
	}

	return 0;
}
int CHOST_Client_send_packet(lua_State *L)
{
	//first, construct a struct packet. then, turn it
	//into byte data with packet_write. finally, send it to the client.
	struct game *game = lua_State_get_game(L);
	struct server *server = game->server;
	struct client *client = NULL;
	bool found_client = false;
	struct packet *packet = lua_State_get_packet(L);
	int client_id = lua_tonumber(L, -2);
	int i;
	int size;
	uint8_t *data;

	if(!packet)
	{
		return 0;
	}

	lua_pop(L, -2);

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
		data = packet_write(packet, &size);
		client_send_packet_data(client, data, size);
	}

	return 0;
}
int CHOST_Client_get_player(lua_State *L)
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
		if(server->clients[i]->id == client_id)
		{
			lua_push_player_object(L, server->clients[i]->player);
			return 1;
		}
	}

	return 0;
}
