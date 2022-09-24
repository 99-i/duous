#include "client.h"
#include "server.h"
#include "id.h"
#include "logger.h"
#include <stdlib.h>

struct client *client_create(struct server *server)
{
	struct client *client = malloc(sizeof(*client));
	client->id = id();
	client->server = server;
	client->state = HANDSHAKING;
	return client;
}

static void client_destroy_callback(uv_handle_t *handle);
void client_destroy(struct client *client)
{
	uv_close((uv_handle_t *) &client->tcp_handle, client_destroy_callback);
}
static void client_destroy_callback(uv_handle_t *handle)
{
	struct client *client = (struct client *) ((char *) handle - offsetof(struct client, tcp_handle));
	free(client);
}