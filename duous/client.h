#pragma once
#include <uv.h>

struct server;

typedef enum client_state_e
{
    STATUS,
    HANDSHAKING,
    LOGIN,
    PLAY,
} client_state;

struct client
{
    int id;
    uv_tcp_t tcp_handle;
    struct server *server;
    client_state state;
};

struct client *client_create(struct server *server);

void client_send_packet_data(struct client *client, uint8_t *packet_data, int packet_data_size);
void client_destroy(struct client *client);