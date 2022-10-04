#pragma once
#include <lua.h>
#include <lauxlib.h>
#include "client.h"
#include "types.h"


#define BEGINLIB(name) const luaL_Reg name ##_lib[] = {
#define LIBFUNCTION(name, f) {#f, CHOST_ ##name ##_ ##f},
#define ENDLIB() {NULL, NULL}};

#define DEFINELIB(name, n) extern const luaL_Reg name ##_lib[n + 1];

struct game *lua_State_get_game(lua_State *L);
const char *client_state_to_cstr(client_state state);
client_state cstr_to_client_state(const char *state);
void lua_push_packet_object(lua_State *L, struct packet *packet);
void lua_push_client_object(lua_State *L, struct client *client);
void lua_push_player_object(lua_State *L, struct player *player);
void lua_push_position_object(lua_State *L, position pos);
void lua_push_look_object(lua_State *L, look look);

void lua_set_game_key(lua_State *L, struct game *game);

