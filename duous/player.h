#pragma once
#include "client.h"
#include "types.h"
#include <stdbool.h>

struct player
{
	char *username;
	struct client *client;
	int id;
	position pos;
	look look;
	bool on_ground;
	
	//zero-based index
	int8_t current_slot;

};

struct player *player_create(struct client *client);

void player_set_username(struct player *player, const char *username);

void player_destroy(struct player *player);