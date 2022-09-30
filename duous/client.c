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
	client->player = NULL;
	return client;
}


struct write_context
{
	uv_write_t req;
	uv_buf_t *buf;
};

void on_write_end(uv_write_t *req, int status)
{
	struct write_context *context = (struct write_context *) req;
	free(context->buf->base);
	free(context->buf);
	free(context);
}

void client_send_packet_data(struct client *client, uint8_t *packet_data, int packet_data_size)
{
	struct write_context *context = malloc(sizeof(*context));
	uv_buf_t *buf = malloc(sizeof(*buf));

	buf->len = packet_data_size;
	buf->base = malloc(packet_data_size);

	memcpy(buf->base, packet_data, packet_data_size);
	context->buf = buf;

	uv_write(&context->req, &client->tcp_handle, buf, 1, on_write_end);
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