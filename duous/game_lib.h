#pragma once
#include "lib.h"
#include <lua.h>
#include <lauxlib.h>

int CHOST_Game_get_max_players(lua_State *L);
int CHOST_Game_set_max_players(lua_State *L);
int CHOST_Game_get_num_players(lua_State *L);
int CHOST_Game_add_player(lua_State *L);

DEFINELIB(game, 4)

