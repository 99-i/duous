#pragma once
#include "client.h"

struct player
{
    char *username;
    struct client *client;
};

struct player *player_create(struct client *client);

void player_set_username(struct player *player, const char *username);

void player_destroy(struct player *player);