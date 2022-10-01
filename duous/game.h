#pragma once
#include "packet.h"
#include "player.h"
#include <lua.h>
#include <uv.h>

struct server;
struct game
{
	int id;

	lua_State *L;
	bool lua_ready;
	uv_loop_t game_loop;
	uv_timer_t tick_timer;

	int update_counter;
	int tick_count;

	int num_players;
	struct player **players;

	int max_players;
	struct server *server;
};

/* constructs a game object. only used by server_create(). */
struct game *game_create(void);

void game_start(struct game *game);

void game_restart_lua(struct game *game);

void game_new_player(struct game *game, struct client *client);
void game_player_disconnected(struct game *game, struct client *client);

/* this function is responsible for freeing packet objects */
void game_get_packet(struct game *game, struct client *client, struct packet *packet);
