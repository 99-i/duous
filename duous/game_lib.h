#pragma once
#include "lib.h"
#include <lua.h>
#include <lauxlib.h>

int CHOST_Game_get_max_players(lua_State *L);
int CHOST_Game_set_max_players(lua_State *L);
int CHOST_Game_get_num_players(lua_State *L);
int CHOST_Game_add_player(lua_State *L);


BEGINLIB(game)
LIBFUNCTION(Game, get_max_players)
LIBFUNCTION(Game, set_max_players)
LIBFUNCTION(Game, get_num_players)
LIBFUNCTION(Game, add_player)
ENDLIB()
