#pragma once
#include <lua.h>
#include <lauxlib.h>

#include "lib.h"

int CHOST_Client_get_state(lua_State *L);
int CHOST_Client_set_state(lua_State *L);
int CHOST_Client_send_packet(lua_State *L);
int CHOST_Client_get_player(lua_State *L);

BEGINLIB(client)
LIBFUNCTION(Client, get_state)
LIBFUNCTION(Client, set_state)
LIBFUNCTION(Client, send_packet)
LIBFUNCTION(Client, get_player)
ENDLIB()
