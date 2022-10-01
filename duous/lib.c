#include "lib.h"
#include "client.h"
#include "game.h"
#include "logger.h"
#include <lua.h>
#include <lauxlib.h>
#include <assert.h>

const char *client_state_to_cstr(client_state state)
{
	switch(state)
	{
		case STATUS:
			return "STATUS";
		case HANDSHAKING:
			return "HANDSHAKING";
		case LOGIN:
			return "LOGIN";
		case PLAY:
			return "PLAY";
		default:
			return "UNDEFINED";
	}
}
client_state cstr_to_client_state(const char *state)
{
	if(!strcmp(state, "STATUS"))
	{
		return STATUS;
	}
	else if(!strcmp(state, "HANDSHAKING"))
	{
		return HANDSHAKING;
	}
	else if(!strcmp(state, "LOGIN"))
	{
		return LOGIN;
	}
	else if(!strcmp(state, "PLAY"))
	{
		return PLAY;
	}
	else
	{
		return -1;
	}
}


void lua_push_packet_object(lua_State *L, struct packet *packet)
{
	const char *direction_str = packet_direction_str(packet->direction);
	int i;
	int j;
	lua_newtable(L);
	lua_pushstring(L, "id");
	lua_pushnumber(L, packet->id);
	lua_settable(L, -3);
	lua_pushstring(L, "direction");
	lua_pushstring(L, direction_str);
	lua_settable(L, -3);
	lua_pushstring(L, "name");
	lua_pushstring(L, packet->name);
	lua_settable(L, -3);

	lua_newtable(L);
	for(i = 0; i < map_size(packet->data); i++)
	{
		char *key = map_getki(packet->data, i);
		data_value *v = map_getvi(packet->data, i);
		lua_pushstring(L, key);
		switch(v->type)
		{
			case DTBOOL:
				lua_pushboolean(L, v->_bool);
				break;
			case DTBYTE:
				lua_pushnumber(L, v->_byte);
				break;
			case DTUNSIGNED_BYTE:
				lua_pushnumber(L, v->_unsigned_byte);
				break;
			case DTSHORT:
				lua_pushnumber(L, v->_short);
				break;
			case DTUNSIGNED_SHORT:
				lua_pushnumber(L, v->_unsigned_short);
				break;
			case DTINT:
				lua_pushnumber(L, v->_int);
				break;
			case DTLONG:
				lua_pushnumber(L, v->_long);
				break;
			case DTFLOAT:
				lua_pushnumber(L, v->_float);
				break;
			case DTDOUBLE:
				lua_pushnumber(L, v->_double);
				break;
			case DTVARINT:
				lua_pushnumber(L, v->_varint);
				break;
			case DTVARLONG:
				lua_pushnumber(L, v->_varlong);
				break;
			case DTSTRING:
				lua_pushstring(L, v->_string);
				break;
			case DTPOSITION:
				LOGFMT(LUATAG(), ("Not supported yet."));
				break;
			case DTUUID:
				LOGFMT(LUATAG(), ("Not supported yet."));
				break;
			case DTBYTE_ARRAY:
				lua_newtable(L);
				for(j = 0; j < v->_byte_array.size; j++)
				{
					lua_pushnumber(L, v->_byte_array.data[i]);
					lua_rawseti(L, -2, i + 1);
				}
				break;
			default:
				assert(false && "Unsupported");
				break;
		}
		lua_settable(L, -3);
	}
	lua_pushstring(L, "fields");
	lua_insert(L, -2);
	lua_settable(L, -3);
}
void lua_push_client_object(lua_State *L, struct client *client)
{
	lua_pushnumber(L, client->id);
}
void lua_push_player_object(lua_State *L, struct player *player)
{
	lua_pushnumber(L, player->id);
}
void lua_push_position_object(lua_State *L, position pos)
{
	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, pos.x);
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, pos.y);
	lua_settable(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, pos.z);
	lua_settable(L, -3);
}
void lua_push_look_object(lua_State *L, look look)
{
	lua_newtable(L);
	lua_pushstring(L, "yaw");
	lua_pushnumber(L, look.yaw);
	lua_settable(L, -3);
	lua_pushstring(L, "pitch");
	lua_pushnumber(L, look.pitch);
	lua_settable(L, -3);
}

static const char game_key = 'g';

struct game *lua_State_get_game(lua_State *L)
{
	lua_pushlightuserdata(L, (void *) &game_key);
	lua_gettable(L, LUA_REGISTRYINDEX);
	struct game *g = lua_topointer(L, -1);

	lua_pop(L, 1);
	return g;
}

void lua_set_game_key(lua_State *L, struct game *game)
{
	lua_pushlightuserdata(game->L, (void *) &game_key);
	lua_pushlightuserdata(game->L, (void *) game);
	lua_settable(game->L, LUA_REGISTRYINDEX);
}
