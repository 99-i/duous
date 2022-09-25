#include "game.h"
#include "id.h"
#include "logger.h"
#include "server.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void dumpstack(lua_State *L)
{
	int top = lua_gettop(L);
	if(top == 0)
	{
		printf("Stack empty.\n");
	}
	for(int i = 1; i <= top; i++)
	{
		printf("%d\t%s\t", i, luaL_typename(L, i));
		switch(lua_type(L, i))
		{
			case LUA_TNUMBER:
				printf("%g\n", lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("%s\n", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf("%s\n", (lua_toboolean(L, i) ? "true" : "false"));
				break;
			case LUA_TNIL:
				printf("%s\n", "nil");
				break;
			default:
				printf("%p\n", lua_topointer(L, i));
				break;
		}
	}
}

struct game *game_create(void)
{
	struct game *game = malloc(sizeof(*game));
	game->id = id();
	game->L = luaL_newstate();
	game->tick_count = 0;
	game->update_counter = 0;

	game->num_players = 0;
	game->players = 0;

	uv_loop_init(&game->game_loop);
	uv_timer_init(&game->game_loop, &game->tick_timer);

	return game;
}
void game_timer_cb(uv_timer_t *timer);
void game_tick(struct game *game);

void game_run_main_lua(struct game *game);
void game_prep_lua(struct game *game);

void game_start(struct game *game)
{
	game_prep_lua(game);
	game_run_main_lua(game);

	uv_timer_start(&game->tick_timer, game_timer_cb, 0, 1000 / 100);
	uv_run(&game->game_loop, UV_RUN_DEFAULT);
}

void game_run_main_lua(struct game *game)
{
	// run all lua files in wildcard 'lib/*.lua'
	WIN32_FIND_DATA find_data;
	HANDLE find_handle;

	find_handle = FindFirstFile("lib/*", &find_data);

	if(find_handle == INVALID_HANDLE_VALUE)
	{
		return;
	}
	do
	{
		if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			char path[400];
			snprintf(path, 400, "lib/%s", find_data.cFileName);
			if(find_data.cFileName[0] != '_')
			{
				luaL_dofile(game->L, path);
			}
		}
	}
	while(FindNextFile(find_handle, &find_data));

	FindClose(find_handle);
}

#pragma region Lua C Host Functions

struct duous_host_function
{
	const char *name;
	lua_CFunction fn;
};

static int CHOST_duous_packet_listener(lua_State *L);

static int CHOST_Client_get_state(lua_State *L);
static int CHOST_Client_set_state(lua_State *L);
static int CHOST_Client_send_packet(lua_State *L);

#pragma endregion


#pragma region Lua C Host Libraries

static const luaL_Reg client_lib[] =
{
	{
	"get_state", CHOST_Client_get_state
	},
	{
		"set_state", CHOST_Client_set_state
	},
	{
		"send_packet",CHOST_Client_send_packet
	},
	{
		NULL, NULL
	}
};

#pragma endregion


struct duous_host_function hosts[] = {{ "__duous_packet_listener",
	CHOST_duous_packet_listener }};

#define PACKET_LISTENER_ARRAY_NAME "__duouspacketlisteners"

static const char game_key = 'g';

struct game *lua_state_get_game(lua_State *L);
void game_prep_lua(struct game *game)
{
	int i;

	lua_pushlightuserdata(game->L, (void *) &game_key);
	lua_pushlightuserdata(game->L, (void *) game);
	lua_settable(game->L, LUA_REGISTRYINDEX);



	luaL_openlibs(game->L);


	lua_newtable(game->L);
	luaL_setfuncs(game->L, client_lib, 0);
	lua_setglobal(game->L, "__Client");

	lua_newtable(game->L);
	lua_setglobal(game->L, PACKET_LISTENER_ARRAY_NAME);

	for(i = 0; i < (sizeof(hosts) / sizeof(hosts[0])); i++)
	{
		struct duous_host_function function = hosts[i];
		lua_register(game->L, function.name, function.fn);
	}

	/*TODO*/
}

static int CHOST_duous_packet_listener(lua_State *L)
{
	int size;
	luaL_checktype(L, 1, LUA_TFUNCTION);

	lua_getglobal(L, PACKET_LISTENER_ARRAY_NAME);

	lua_insert(L, -2);
	size = luaL_len(L, -2);
	lua_rawseti(L, -2, size + 1);
	return 0;
}

const char *client_state_to_cstr(client_state state);
client_state cstr_to_client_state(const char *state);
static int CHOST_Client_get_state(lua_State *L)
{
	struct game *game = lua_state_get_game(L);
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
static int CHOST_Client_set_state(lua_State *L)
{
	struct game *game = lua_state_get_game(L);
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


//gets the struct packet * from a lua state (the packet object on the top of the stack)
struct packet *lua_State_get_packet(lua_State *L);
//clientbound packet object:
//id: number
//values: array of values
//types: array of types
//such that types[i] corresponds to the type of the value in values[i]

static int CHOST_Client_send_packet(lua_State *L)
{
	struct game *game = lua_state_get_game(L);
	struct server *server = game->server;
	struct client *client = NULL;
	bool found_client = false;
	struct packet *packet = lua_State_get_packet(L);
	int client_id = lua_tonumber(L, -2);
	int i;
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
	}

}


struct packet *lua_State_get_packet(lua_State *L)
{
	assert(false && "TODO");
	return NULL;
}

const char *client_state_to_cstr(client_state state)
{
	switch(state)
	{
		case STATUS:
			return "STATUS";
		case HANDSHAKING:
			return "HANDSHAKING";
		case LOGIN:
			return "LOGIN";
		case PLAY:
			return "PLAY";
		default:
			return "UNDEFINED";
	}
}
client_state cstr_to_client_state(const char *state)
{
	if(!strcmp(state, "STATUS"))
	{
		return STATUS;
	}
	else if(!strcmp(state, "HANDSHAKING"))
	{
		return HANDSHAKING;
	}
	else if(!strcmp(state, "LOGIN"))
	{
		return LOGIN;
	}
	else if(!strcmp(state, "PLAY"))
	{
		return PLAY;
	}
	else
	{
		return -1;
	}
}

void game_timer_cb(uv_timer_t *timer)
{
	struct game *game = (struct game *) ((char *) timer - offsetof(struct game, tick_timer));
	game->update_counter++;
	if(game->update_counter % 5 == 0)
	{
		game_tick(game);
	}
}

void game_tick(struct game *game)
{
	game->tick_count++;
}

void game_new_player(struct game *game, struct client *client)
{
	struct player *player = player_create(client);
	game->num_players++;
	game->players = realloc(game->players, sizeof(*game->players) * game->num_players);
	game->players[game->num_players - 1] = player;
	LOGFMT(GAMETAG(), ("Added client with ID %d", client->id));
}

void game_player_disconnected(struct game *game, struct client *client)
{
	int i;
	int idx = -1;
	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->client == client)
		{
			idx = i;
		}
	}
	if(idx == -1)
		return;

	player_destroy(game->players[idx]);

	for(i = idx; i < game->num_players - 1; i++)
	{
		game->players[i] = game->players[i + 1];
	}
	game->num_players--;
	game->players = realloc(game->players, sizeof(*game->players) * game->num_players);
}

void lua_push_packet_object(lua_State *L, struct packet *packet);
void lua_push_client_object(lua_State *L, struct client *client);



void game_get_packet(struct game *game, struct client *client, struct packet *packet)
{
	int i, j, k;
	int size;
	struct player *player = NULL;
	struct form *form = NULL;
	bool found_form = false;

	for(i = 0; i < NUM_FORMS; i++)
	{
		form = &forms[i];
		if(form->id == packet->id && form->state == client->state)
		{
			found_form = true;
			break;
		}
	}

	if(!found_form)
		return;

	for(i = 0; i < game->num_players; i++)
	{
		if(game->players[i]->client == client)
		{
			player = game->players[i];
		}
	}
	if(player == NULL)
		return;

	lua_getglobal(game->L, PACKET_LISTENER_ARRAY_NAME);

	size = luaL_len(game->L, -1);

	for(i = 0; i < size; i++)
	{
		lua_rawgeti(game->L, 1, i + 1);
		lua_push_client_object(game->L, client);
		lua_push_packet_object(game->L, packet);

		lua_call(game->L, 2, 0);
	}
	lua_pop(game->L, 1);
	packet_destroy(packet);
}

void lua_push_packet_object(lua_State *L, struct packet *packet)
{
	const char *direction_str = packet_direction_str(packet->direction);
	int i;
	lua_newtable(L);
	lua_pushstring(L, "id");
	lua_pushnumber(L, packet->id);
	lua_settable(L, -3);
	lua_pushstring(L, "direction");
	lua_pushstring(L, direction_str);
	lua_settable(L, -3);
	lua_pushstring(L, "name");
	lua_pushstring(L, packet->name);
	lua_settable(L, -3);

	lua_newtable(L);
	for(i = 0; i < map_size(packet->data); i++)
	{
		char *key = map_getki(packet->data, i);
		data_value *v = map_getvi(packet->data, i);
		lua_pushstring(L, key);
		switch(v->type)
		{
			case DTBOOL:
				lua_pushboolean(L, v->_bool);
				break;
			case DTBYTE:
				lua_pushnumber(L, v->_byte);
				break;
			case DTUNSIGNED_BYTE:
				lua_pushnumber(L, v->_unsigned_byte);
				break;
			case DTSHORT:
				lua_pushnumber(L, v->_short);
				break;
			case DTUNSIGNED_SHORT:
				lua_pushnumber(L, v->_unsigned_short);
				break;
			case DTINT:
				lua_pushnumber(L, v->_int);
				break;
			case DTLONG:
				lua_pushnumber(L, v->_long);
				break;
			case DTFLOAT:
				lua_pushnumber(L, v->_float);
				break;
			case DTDOUBLE:
				lua_pushnumber(L, v->_double);
				break;
			case DTVARINT:
				lua_pushnumber(L, v->_varint);
				break;
			case DTVARLONG:
				lua_pushnumber(L, v->_varlong);
				break;
			case DTANGLE:
				lua_pushnumber(L, v->_angle);
				break;
			case DTSTRING:
				lua_pushstring(L, v->_string);
				break;
			case DTPOSITION:
				LOGFMT(LUATAG(), ("Not supported yet."));
				break;
			case DTUUID:
				LOGFMT(LUATAG(), ("Not supported yet."));
				break;
			default:
				break;
		}
		lua_settable(L, -3);
	}
	lua_pushstring(L, "fields");
	lua_insert(L, -2);
	lua_settable(L, -3);
}

void lua_push_client_object(lua_State *L, struct client *client)
{
	lua_pushnumber(L, client->id);
}

struct game *lua_state_get_game(lua_State *L)
{
	lua_pushlightuserdata(L, (void *) &game_key);
	lua_gettable(L, LUA_REGISTRYINDEX);
	struct game *g = lua_topointer(L, -1);

	lua_pop(L, 1);
	return g;
}
