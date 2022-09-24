#pragma once
#include "game.h"
#include <uv.h>
#include <stdbool.h>

struct server
{
	int id;

	bool running;

	struct game *game;

	int port;

	uv_loop_t loop;
	uv_thread_t server_run_thread;
	uv_tcp_t tcp_handle;

	int num_clients;
	struct client **clients;

	uv_thread_t game_run_thread;
};

/* creates a server object, and its game object. */
struct server *server_create(int port);

/* props up this server before running all subservers. */
void server_run(struct server *server);
