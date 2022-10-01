#pragma once
#include "lib.h"
#include <lua.h>
#include <lauxlib.h>

int CHOST_Player_get_username(lua_State *L);
int CHOST_Player_set_username(lua_State *L);
int CHOST_Player_get_client(lua_State *L);
int CHOST_Player_get_position(lua_State *L);
int CHOST_Player_set_position(lua_State *L);
int CHOST_Player_get_look(lua_State *L);
int CHOST_Player_set_look(lua_State *L);
int CHOST_Player_get_on_ground(lua_State *L);
int CHOST_Player_set_on_ground(lua_State *L);
int CHOST_Player_get_current_slot(lua_State *L);
int CHOST_Player_set_current_slot(lua_State *L);


BEGINLIB(player)
LIBFUNCTION(Player, get_username)
LIBFUNCTION(Player, set_username)
LIBFUNCTION(Player, get_client)
LIBFUNCTION(Player, get_position)
LIBFUNCTION(Player, set_position)
LIBFUNCTION(Player, get_look)
LIBFUNCTION(Player, set_look)
LIBFUNCTION(Player, get_on_ground)
LIBFUNCTION(Player, set_on_ground)
LIBFUNCTION(Player, get_current_slot)
LIBFUNCTION(Player, set_current_slot)
ENDLIB()
