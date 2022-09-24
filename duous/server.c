#include "server.h"
#include "id.h"
#include "game.h"
#include "logger.h"
#include "main.h"
#include "client.h"
#include "packet.h"
#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

#define SERVER_BACKLOG 128

struct server *server_create(int port)
{
	struct server *server = malloc(sizeof(*server));
	server->id = id();
	server->running = false;
	server->game = game_create();
	server->game->server = server;
	server->port = port;
	server->num_clients = 0;
	server->clients = 0;
	uv_loop_init(&server->loop);
	return server;
}

static void server_loop_start(struct server *server);
static void server_on_connection(uv_stream_t *server_tcp_handle, int status);
static void server_allocate_buffer_callback(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
static void server_read_callback(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

static void server_add_client(struct server *server, struct client *client);
static void server_remove_client(struct server *server, int id);

static void server_handle_read(struct server *server, struct client *client, int data_size, uint8_t *data, bool is_wraparound);

void server_run(struct server *server)
{
	struct sockaddr_in address;
	int len;
	struct sockaddr_in port_addr;
	int port;
	/* TODO: run the server. */
	server->running = true;

	uv_tcp_init(&server->loop, &server->tcp_handle);

	uv_ip4_addr("127.0.0.1", server->port, &address);
	uv_tcp_bind(&server->tcp_handle, (struct sockaddr *) &address, 0);
	uv_listen((uv_stream_t *) &server->tcp_handle, SERVER_BACKLOG, server_on_connection);

	uv_tcp_getsockname(&server->tcp_handle, (struct sockaddr *) &port_addr, &len);

	port = ntohs(port_addr.sin_port);
	if(port != 0)
	{
		server->port = port;
	}

	LOGFMT(SERVERTAG(), ("Running Server-%x on port %d", server->id, server->port));
	uv_thread_create(&server->server_run_thread, (uv_thread_cb) server_loop_start, server);
	application_add_thread(app, &server->server_run_thread);
}

void game_run(void *arg);
static void server_loop_start(struct server *server)
{
	LOGFMT(SERVERTAG(), ("Join Server-%d.", server->id));
	uv_thread_create(&server->game_run_thread, game_run, server);
	uv_run(&server->loop, UV_RUN_DEFAULT);
}

void game_run(void *arg)
{
	struct server *server = (struct server *) arg;

	game_start(server->game);
}

static void server_on_connection(uv_stream_t *server_tcp_handle, int status)
{
	struct server *server = (struct server *) ((char *) server_tcp_handle - offsetof(struct server, tcp_handle));
	int r = 1;
	struct client *client = client_create(server);
	uv_tcp_init(&server->loop, &client->tcp_handle);

	r = uv_accept((uv_stream_t *) &server->tcp_handle, (uv_stream_t *) &client->tcp_handle);

	server_add_client(server, client);
	if(r)
	{
		LOGFMT(SERVERTAG(), ("Encountered error."));
		uv_close((uv_handle_t *) &client->tcp_handle, NULL);
		free(client);
		return;
	}
	uv_read_start((uv_stream_t *) &client->tcp_handle, server_allocate_buffer_callback, server_read_callback);
}

static void server_allocate_buffer_callback(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
	buf->base = malloc(suggested_size);
	buf->len = suggested_size;
}

static void server_read_callback(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	struct client *client = (struct client *) ((char *) stream - offsetof(struct client, tcp_handle));
	struct server *server = client->server;
	if(nread == UV_EOF)
	{
		server_remove_client(server, client->id);
		game_player_disconnected(server->game, client);
		client_destroy(client);
		if(buf->base)
		{
			free(buf->base);
		}
		return;
	}
	if(buf->base)
	{
		server_handle_read(server, client, nread, (uint8_t *) buf->base, false);
		free(buf->base);
	}
}

static void server_add_client(struct server *server, struct client *client)
{
	server->num_clients++;
	server->clients = realloc(server->clients, sizeof(*server->clients) * server->num_clients);
	server->clients[server->num_clients - 1] = client;
	client->server = server;
	game_new_player(server->game, client);
}

static void server_remove_client(struct server *server, int id)
{
	int i;
	int index = -1;
	for(i = 0; i < server->num_clients; i++)
	{
		if(server->clients[i]->id == id)
		{
			index = i;
		}
	}
	if(index == -1)
		return;
	for(i = index; i < server->num_clients - 1; i++)
	{
		server->clients[i] = server->clients[i + 1];
	}
	server->num_clients--;
	server->clients = realloc(server->clients, sizeof(*server->clients) * server->num_clients);
}

static void server_get_packet(struct server *server, struct client *client, struct packet *packet);
static void server_handle_read(struct server *server, struct client *client, int data_size, uint8_t *data, bool is_wraparound)
{
	struct wraparound wraparound;
	struct packet *packet = malloc(sizeof(*packet));
	int i;
	bool result;
	if(!is_wraparound)
	{
		if(should_wraparound(data, data_size, &wraparound))
		{
			free(packet);
			server_handle_read(server, client, wraparound.first_size, data, true);
			server_handle_read(server, client, wraparound.second_size, data + wraparound.cutoff, true);
			return;
		}
		else
		{
			goto read_packet;
		}
	}
read_packet:
	result = packet_read(packet, data, data_size, client->state);
	if(!result)
	{
		server_remove_client(server, client->id);
		uv_read_stop((uv_stream_t *) &client->tcp_handle);
		client_destroy(client);
		return;
	}
	server_get_packet(server, client, packet);
}

static void server_get_packet(struct server *server, struct client *client, struct packet *packet)
{
	struct game *game = server->game;
	game_get_packet(game, client, packet);
}
