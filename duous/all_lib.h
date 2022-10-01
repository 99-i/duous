#pragma once

#include "client_lib.h"
#include "player_lib.h"
#include "game_lib.h"

#define MAKELIB(name, reserved_name) lua_newtable(game->L);\
	luaL_setfuncs(game->L, name ##_lib, 0);\
	lua_setglobal(game->L, #reserved_name)
