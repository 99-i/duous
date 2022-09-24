#define _CRT_SECURE_NO_WARNINGS
#include "player.h"
#include <stdlib.h>
#include <string.h>

struct player *player_create(struct client *client)
{
    struct player *player = malloc(sizeof(*player));
    player->client = client;
    player->username = NULL;
    return player;
}

void player_set_username(struct player *player, const char *username)
{
    player->username = malloc(strlen(username) + 1);
    strcpy(player->username, username);
}

void player_destroy(struct player *player)
{
    free(player->username);
    free(player);
}