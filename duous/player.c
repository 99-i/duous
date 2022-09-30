#define _CRT_SECURE_NO_WARNINGS
#include "player.h"
#include "id.h"
#include <stdlib.h>
#include <string.h>

struct player *player_create(struct client *client)
{
    struct player *player = malloc(sizeof(*player));
    player->client = client;
    player->username = NULL;
    player->id = id();
    player->pos.x = 0;
    player->pos.y = 0;
    player->pos.z = 0;

    player->look.pitch = 0;
    player->look.yaw = 0;

    player->current_slot = 0;

    player->on_ground = false;

    return player;
}

void player_set_username(struct player *player, const char *username)
{
    free(player->username);
    player->username = malloc(strlen(username) + 1);
    strcpy(player->username, username);
}

void player_destroy(struct player *player)
{
    free(player->username);
    free(player);
}