#include "game.h"
#include "id.h"
#include "logger.h"
#include "server.h"
#include "lfs.h"
#include "all_lib.h"
#include "world.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void dumpstack(lua_State *L)
{
	printf("---------------------\n");
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
	game->lua_ready = false;
	game->L = luaL_newstate();
	game->tick_count = 0;
	game->update_counter = 0;

	game->num_players = 0;
	game->players = 0;

	game->max_players = 100;
	game->world = world_create();

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

void game_restart_lua(struct game *game)
{
	if(!game->lua_ready) return;
	game->lua_ready = false;
	LOGFMT(GAMETAG(), ("Restarting Lua-Engine..."));
	lua_close(game->L);
	game->L = luaL_newstate();
	game_prep_lua(game);
	game_run_main_lua(game);
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
	game->lua_ready = true;
}

#pragma region Lua C Host Functions

struct duous_host_function
{
	const char *name;
	lua_CFunction fn;
};

static int CHOST_duous_packet_listener(lua_State *L);
static int CHOST_print(lua_State *L);

#pragma endregion



struct duous_host_function hosts[] =
{
	{
		"__duous_packet_listener",
		CHOST_duous_packet_listener
	},
	{
		"print",
		CHOST_print
	}
};



#define PACKET_LISTENER_ARRAY_NAME "__duouspacketlisteners"

void game_prep_lua(struct game *game)
{
	int i;

	lua_set_game_key(game->L, game);


	luaL_openlibs(game->L);

	MAKELIB(client, __Client);
	MAKELIB(player, __Player);
	MAKELIB(game, __Game);

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

static int CHOST_print(lua_State *L)
{
	luaL_checkstring(L, -1);
	const char *str = lua_tostring(L, -1);
	LOGFMT(("Lua-Engine"), ("%s", str));
	return 0;
}


//gets the struct packet * from a lua state (the packet object on the top of the stack)
//clientbound packet object:
//id: number
//values: array of values
//types: array of types
//such that types[i] corresponds to the type of the value in values[i]
struct packet *lua_State_get_packet(lua_State *L);






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

	client->player = player;

	game->players = realloc(game->players, sizeof(*game->players) * game->num_players);
	game->players[game->num_players - 1] = player;
}

void game_player_disconnected(struct game *game, struct client *client)
{
	int i;
	int idx = -1;

	if(client->player == NULL)
		return;
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



void game_get_packet(struct game *game, struct client *client, struct packet *packet)
{
	int i, j, k;
	int size;

	if(!game->lua_ready) return;
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

