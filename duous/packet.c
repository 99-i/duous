#define _CRT_SECURE_NO_WARNINGS
#include "packet.h"
#include "logger.h"
#include "client.h"
#include <string.h>
#include <lua.h>
#include <assert.h>


/*

	first read the length.
	then read the id.
	use the id to read all the fields.
	uint8_t *data_pointer: an offset from data to read from.

*/



const char *packet_direction_str(packet_direction pd)
{
	switch(pd)
	{
		case SERVERBOUND:
			return "SERVERBOUND";
		case CLIENTBOUND:
			return "CLIENTBOUND";
	}
	return "Undefined";
}

typedef bool (*data_read_fn_pointer)(uint8_t *data, int read_max, void *dest, int *size);
static data_read_fn_pointer read_pointers[NUM_DATA_TYPES] = {

	[DTBOOL] = (data_read_fn_pointer) __read_bool,

	[DTBYTE] = (data_read_fn_pointer) __read_byte,

	[DTUNSIGNED_BYTE] = (data_read_fn_pointer) __read_unsigned_byte,

	[DTSHORT] = (data_read_fn_pointer) __read_short,

	[DTUNSIGNED_SHORT] = (data_read_fn_pointer) __read_unsigned_short,

	[DTINT] = (data_read_fn_pointer) __read_int,

	[DTLONG] = (data_read_fn_pointer) __read_long,

	[DTFLOAT] = (data_read_fn_pointer) __read_float,

	[DTDOUBLE] = (data_read_fn_pointer) __read_double,

	[DTSTRING] = (data_read_fn_pointer) __read_string,

	[DTVARINT] = (data_read_fn_pointer) __read_varint,

	[DTVARLONG] = (data_read_fn_pointer) __read_varlong,

	[DTPOSITION] = (data_read_fn_pointer) __read_position,

	[DTANGLE] = (data_read_fn_pointer) __read_angle,

	[DTUUID] = (data_read_fn_pointer) __read_uuid,

};


#define READ(type, dest)                                                                                  \
    do                                                                                                    \
    {                                                                                                     \
        int read_size;                                                                                    \
        bool r = read_pointers[type](data_pointer, data_size - (data_pointer - data), &dest, &read_size); \
        if (!r)                                                                                           \
        {                                                                                                 \
            return false;                                                                                 \
        }																								  \
        data_pointer += read_size;                                                                        \
    } while (false)

#define SETPACKETDATA(type, dest) \
    READ(type, dest);             \
    v->dest = dest

bool packet_read(struct packet *packet, uint8_t *data, int data_size, client_state state)
{
	uint8_t *data_pointer = data;
	int offset = 0;
	int32_t length;
	int32_t id;
	int i;
	bool found_form = false;
	struct form *form = NULL;

	bool _bool;
	int8_t _byte;
	uint8_t _unsigned_byte;
	int16_t _short;
	uint16_t _unsigned_short;
	int32_t _int;
	int64_t _long;
	float _float;
	double _double;
	char *_string;
	int32_t _varint;
	int64_t _varlong;
	position _position;
	uint8_t _angle;
	uuid _uuid;

	packet->data = map_create(free_data_value);

	READ(DTVARINT, length);
	READ(DTVARINT, id);

	packet->direction = SERVERBOUND;
	packet->id = id;

	for(i = 0; i < NUM_FORMS; i++)
	{
		form = &forms[i];
		if(form->id == id && form->state == state)
		{
			found_form = true;
			break;
		}
	}

	if(!found_form)
	{
		switch(state)
		{
			case PLAY:
				switch(id)
				{
					case 0x08:
						//Player Block Placement
						break;
					case 0x0e:
						//Click Window
						break;
					case 0x10:
						//Creative Inventory Action
						break;
					case 0x17:
						//Plugin Message
					{
						data_value *v = malloc(sizeof(*v));
						v->type = DTSTRING;
						SETPACKETDATA(DTSTRING, _string);
						map_set(packet->data, "Channel", v);

						v = malloc(sizeof(*v));
						v->type = DTSHORT;
						SETPACKETDATA(DTSHORT, _short);
						map_set(packet->data, "Length", v);

						v = malloc(sizeof(*v));
						byte_array ba;
						v->type = DTBYTE_ARRAY;
						ba.size = _short;
						int read_size;
						bool r = __read_byte_array(data_pointer, data_size - (data_pointer - data), &ba.data, &read_size, _short);
						if(!r)
						{
							return false;
						}
						v->_byte_array = ba;
						map_set(packet->data, "Data", v);
						data_pointer += read_size;

						packet->name = malloc(strlen("Plugin Message") + 1);
						strcpy(packet->name, "Plugin Message");
						return true;

					}
					break;
					default:
						break;
				}
				break;
			case LOGIN:
				switch(id)
				{
					case 0x01:
						//Encryption Response
						break;
					default:
						break;
				}
			default:
				break;
		}

		return false;
	}

	for(i = 0; i < form->num_fields; i++)
	{
		data_type type;
		data_value *v;
		type = form->fields[i].type;
		if(form->fields[i].name == NULL && form->fields[i].type == NULL)
		{
			break;
		}
		v = malloc(sizeof(*v));
		v->type = type;
		switch(type)
		{
			case DTBOOL:
				SETPACKETDATA(DTBOOL, _bool);
				break;
			case DTBYTE:
				SETPACKETDATA(DTBYTE, _byte);
				break;
			case DTUNSIGNED_BYTE:
				SETPACKETDATA(DTUNSIGNED_BYTE, _unsigned_byte);
				break;
			case DTSHORT:
				SETPACKETDATA(DTSHORT, _short);
				break;
			case DTUNSIGNED_SHORT:
				SETPACKETDATA(DTUNSIGNED_SHORT, _unsigned_short);
				break;
			case DTINT:
				SETPACKETDATA(DTINT, _int);
				break;
			case DTLONG:
				SETPACKETDATA(DTLONG, _long);
				break;
			case DTFLOAT:
				SETPACKETDATA(DTFLOAT, _float);
				break;
			case DTDOUBLE:
				SETPACKETDATA(DTDOUBLE, _double);
				break;
			case DTSTRING:
				SETPACKETDATA(DTSTRING, _string);
				break;
			case DTVARINT:
				SETPACKETDATA(DTVARINT, _varint);
				break;
			case DTVARLONG:
				SETPACKETDATA(DTVARLONG, _varlong);
				break;
			case DTPOSITION:
				SETPACKETDATA(DTPOSITION, _position);
				break;
			case DTANGLE:
				SETPACKETDATA(DTANGLE, _angle);
				break;
			case DTUUID:
				READ(DTUUID, _uuid);
				memcpy(&v->_uuid, &_uuid, sizeof(_uuid));
				break;
			default:
				break;
		}
		map_set(packet->data, form->fields[i].name, v);
	}
	packet->name = malloc(sizeof(*packet->name) * strlen(form->packet_name) + 1);
	strcpy(packet->name, form->packet_name);
	return true;
}
#undef READ


#define _WRITE(type, v) holder = __write_##type(v, &holder_size);\
					size += holder_size;\
					data = realloc(data, size);\
					memcpy(data + data_needle, holder, holder_size);\
					data_needle = size;\
					free(holder)
#define WRITE(type) _WRITE(##type, value->_##type)
uint8_t *packet_write(struct packet *packet, int *data_size)
{
	int i;
	int size = 0;
	int data_needle = 0;
	uint8_t *data = malloc(size);
	uint8_t *holder;
	int holder_size;


	_WRITE(varint, packet->id);



	for(i = 0; i < map_size(packet->data); i++)
	{
		data_value *value = map_getvi(packet->data, i);
		switch(value->type)
		{
			case DTBOOL:
				WRITE(bool);
				break;
			case DTBYTE:
				WRITE(byte);
				break;
			case DTUNSIGNED_BYTE:
				WRITE(unsigned_byte);
				break;
			case DTSHORT:
				WRITE(short);
				break;
			case DTUNSIGNED_SHORT:
				WRITE(unsigned_short);
				break;
			case DTINT:
				WRITE(int);
				break;
			case DTLONG:
				WRITE(long);
				break;
			case DTFLOAT:
				WRITE(float);
				break;
			case DTDOUBLE:
				WRITE(double);
				break;
			case DTSTRING:
				WRITE(string);
				break;
			case DTVARINT:
				WRITE(varint);
				break;
			case DTVARLONG:
				WRITE(varlong);
				break;
			case DTPOSITION:
				WRITE(position);
				break;
			case DTANGLE:
				WRITE(angle);
				break;
			case DTUUID:
				WRITE(uuid);
				break;
			default:
				break;
		}
	}


	holder = __write_varint(size, &holder_size);

	memmove(data + holder_size, data, size);

	memcpy(data, holder, holder_size);
	size += holder_size;


	if(data_size)
		*data_size = size;

	return data;
}

data_type packet_cstr_to_data_type(const char *type);
struct packet *lua_State_get_packet(lua_State *L)
{
	bool _bool;
	int8_t _byte;
	uint8_t _unsigned_byte;
	int16_t _short;
	uint16_t _unsigned_short;
	int32_t _int;
	int64_t _long;
	float _float;
	double _double;
	char *_string;
	int32_t _varint;
	int64_t _varlong;
	position _position;
	uint8_t _angle;
	uuid _uuid;

	char map_index[256] = "+";

	struct packet *packet = malloc(sizeof(*packet));
	int id;
	int num_fields;
	int i;

	packet->direction = CLIENTBOUND;
	packet->data = map_create(free_data_value);

	lua_pushstring(L, "id");
	lua_gettable(L, -2);
	if(lua_isnumber(L, -1))
		id = lua_tonumber(L, -1);
	else
	{
		packet_destroy(packet);
		return NULL;
	}
	packet->id = id;
	lua_pop(L, 1);

	lua_pushstring(L, "values");
	lua_gettable(L, -2);
	lua_len(L, -1);
	num_fields = lua_tonumber(L, -1);
	lua_pop(L, 2);

	for(i = 0; i < num_fields; i++)
	{
		data_value *value = malloc(sizeof(*value));
		data_type current_type;
		const char *current_type_str;
		int type;
		lua_pushstring(L, "types");
		lua_gettable(L, -2);
		lua_rawgeti(L, -1, i + 1);
		if(!lua_isstring(L, -1))
		{
			packet_destroy(packet);
			return NULL;
		}
		current_type_str = lua_tostring(L, -1);
		lua_pop(L, 1);
		current_type = packet_cstr_to_data_type(current_type_str);
		lua_pop(L, 1);

		lua_pushstring(L, "values");
		lua_gettable(L, -2);
		lua_rawgeti(L, -1, i + 1);

		value->type = current_type;
#define MAPSET(f, vn)	_##vn = ##f(L, -1);\
						strcat(map_index, "+");\
						value->_##vn = _##vn;\
						map_set(packet->data, map_index, value)
		switch(current_type)
		{
			case DTBOOL:
				MAPSET(lua_toboolean, bool);
				break;
			case DTBYTE:
				MAPSET(lua_tonumber, byte);
				break;
			case DTUNSIGNED_BYTE:
				MAPSET(lua_tonumber, unsigned_byte);
				break;
			case DTSHORT:
				MAPSET(lua_tonumber, short);
				break;
			case DTUNSIGNED_SHORT:
				MAPSET(lua_tonumber, unsigned_short);
				break;
			case DTINT:
				MAPSET(lua_tonumber, int);
				break;
			case DTLONG:
				MAPSET(lua_tonumber, long);
				break;
			case DTFLOAT:
				MAPSET(lua_tonumber, float);
				break;
			case DTDOUBLE:
				MAPSET(lua_tonumber, double);
				break;
			case DTSTRING:
				_string = _strdup(lua_tostring(L, -1));
				strcat(map_index, "+");
				value->_string = _string;
				map_set(packet->data, map_index, value);
				break;
			case DTVARINT:
				MAPSET(lua_tonumber, varint);
				break;
			case DTVARLONG:
				MAPSET(lua_tonumber, varlong);
				break;
			case DTPOSITION:
				assert(false && "Unimplemented.");
				break;
			case DTANGLE:
				MAPSET(lua_tonumber, angle);
				break;
			case DTUUID:
				assert(false && "Unimplemented.");
				break;
			default:
				break;
		}
#undef MAPSET

		lua_pop(L, 2);
	}


	return packet;
}

data_type packet_cstr_to_data_type(const char *type)
{
	if(!strcmp(type, "boolean"))
		return DTBOOL;
	else if(!strcmp(type, "byte"))
		return DTBYTE;
	else if(!strcmp(type, "unsigned_byte"))
		return DTUNSIGNED_BYTE;
	else if(!strcmp(type, "short"))
		return DTSHORT;
	else if(!strcmp(type, "unsigned_short"))
		return DTUNSIGNED_SHORT;
	else if(!strcmp(type, "int"))
		return DTINT;
	else if(!strcmp(type, "long"))
		return DTLONG;
	else if(!strcmp(type, "float"))
		return DTFLOAT;
	else if(!strcmp(type, "double"))
		return DTDOUBLE;
	else if(!strcmp(type, "string"))
		return DTSTRING;
	else if(!strcmp(type, "varint"))
		return DTVARINT;
	else if(!strcmp(type, "varlong"))
		return DTVARLONG;
	else if(!strcmp(type, "position"))
		return DTPOSITION;
	else if(!strcmp(type, "angle"))
		return DTANGLE;
	return -1;
}


bool __read_byte_array(uint8_t *data, int read_max, uint8_t **_byte_array, int *size, int array_size)
{
	if(read_max < array_size)
		return false;

	if(size)
		*size = array_size;

	*_byte_array = malloc(array_size);
	memcpy(*_byte_array, data, array_size);

	return true;
}

bool __read_bool(uint8_t *data, int read_max, bool *_bool, int *size)
{
	if(read_max < 1)
		return false;
	if(_bool)
		*_bool = data[0];
	if(size)
		*size = 1;
	return true;
}
bool __read_byte(uint8_t *data, int read_max, int8_t *_byte, int *size)
{
	if(read_max < 1)
		return false;
	if(_byte)
		*_byte = data[0];
	if(size)
		*size = 1;
	return true;
}
bool __read_unsigned_byte(uint8_t *data, int read_max, uint8_t *_unsigned_byte, int *size)
{
	if(read_max < 1)
		return false;
	if(_unsigned_byte)
		*_unsigned_byte = data[0];
	if(size)
		*size = 1;
	return true;
}
bool __read_short(uint8_t *data, int read_max, int16_t *_short, int *size)
{
	if(read_max < 2)
		return false;
	if(_short)
	{
		*_short = 0;
		*_short = (data[0] << 8) | data[1];
	}
	if(size)
		*size = 2;
	return true;
}
bool __read_unsigned_short(uint8_t *data, int read_max, uint16_t *_unsigned_short, int *size)
{
	if(read_max < 2)
		return false;
	if(_unsigned_short)
	{
		*_unsigned_short = 0;
		*_unsigned_short = (data[0] << 8) | data[1];
	}
	if(size)
		*size = 2;
	return true;
}
bool __read_int(uint8_t *data, int read_max, int32_t *_int, int *size)
{
	if(read_max < 4)
		return false;
	if(_int)
	{
		*_int = 0;
		*_int = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	}
	if(size)
		*size = 4;
	return true;
}
bool __read_long(uint8_t *data, int read_max, int64_t *_long, int *size)
{
	if(read_max < 8)
		return false;
	if(_long)
	{
		*_long = 0;
		*_long = (((uint64_t) data[0]) << 56) |
			(((uint64_t) data[1]) << 48) |
			(((uint64_t) data[2]) << 40) |
			(((uint64_t) data[3]) << 32) |
			(((uint64_t) data[4]) << 24) |
			(((uint64_t) data[5]) << 16) |
			(((uint64_t) data[6]) << 8) |
			((uint64_t) data[7]);
	}
	if(size)
		*size = 8;
	return true;
}
bool __read_float(uint8_t *data, int read_max, float *_float, int *size)
{
	int32_t i;
	if(read_max < 4)
		return false;

	__read_int(data, read_max, &i, NULL);

	if(_float)
		*_float = *((float *) &i);
	if(size)
		*size = 4;
	return true;
}
bool __read_double(uint8_t *data, int read_max, double *_double, int *size)
{
	int64_t l;
	if(read_max < 8)
		return false;

	__read_long(data, read_max, &l, NULL);

	if(_double)
		*_double = *((double *) &l);
	if(size)
		*size = 8;
	return true;
}
bool __read_string(uint8_t *data, int read_max, char **string, int *size)
{
	int32_t str_length;
	int i;
	char *str;
	int32_t varint_length;
	__read_varint(data, read_max, &str_length, &varint_length);

	if(string)
	{
		*string = malloc(sizeof(**string) * str_length + 1);
		for(i = 0; i < str_length; i++)
		{
			if(i + varint_length > read_max)
			{
				free(*string);
				return false;
			}
			(*string)[i] = data[i + varint_length];
		}
		(*string)[str_length] = 0;
	}
	if(size)
		*size = str_length + varint_length;
	return true;
}
bool __read_varint(uint8_t *data, int read_max, int32_t *varint, int *size)
{
	int32_t decoded_int = 0;
	int32_t bit_offset = 0;
	int32_t current_byte = 0;
	int32_t len = 0;
	do
	{
		if(len >= read_max)
		{
			return false;
		}
		current_byte = data[len];

		decoded_int |= (current_byte & 0b01111111) << bit_offset;

		if(bit_offset == 35)
		{
			return false;
		}
		bit_offset += 7;
		len++;
	}
	while((current_byte & 0b10000000) != 0);

	if(varint)
		*varint = decoded_int;
	if(size)
		*size = len;
	return true;
}
bool __read_varlong(uint8_t *data, int read_max, int64_t *_varlong, int *size)
{
	int64_t decoded_long = 0;
	int32_t bit_offset = 0;
	int32_t current_byte = 0;
	int32_t len = 0;
	do
	{
		if(len >= read_max)
		{
			return false;
		}
		current_byte = data[len];

		decoded_long |= ((uint64_t) (current_byte & 0b01111111)) << bit_offset;

		if(bit_offset == 50)
		{
			return false;
		}
		bit_offset += 7;
		len++;
	}
	while((current_byte & 0b10000000) != 0);

	if(_varlong)
		*_varlong = decoded_long;
	if(size)
		*size = len;
	return true;
}
bool __read_position(uint8_t *data, int read_max, position *_position, int *size)
{
	int64_t l;
	if(read_max < 8)
		return false;

	__read_long(data, read_max, &l, NULL);

	if(_position)
	{
		_position->x = (l >> 38);
		_position->y = (l & 0xFF);
		_position->z = (l >> 12) & 0x3FFFFFF;
	}

	if(size)
		*size = 8;
	return true;
}
bool __read_angle(uint8_t *data, int read_max, int8_t *_angle, int *size)
{
	if(read_max < 1)
		return false;
	if(_angle)
		*_angle = data[0];
	if(size)
		*size = 1;
	return true;
}
bool __read_uuid(uint8_t *data, int read_max, uuid *uuid, int *size)
{
	if(read_max < 16)
		return false;

	if(uuid)
	{
		__read_long(data, read_max, &uuid[0], NULL);
		__read_long(data + 8, read_max - 8, &uuid[1], NULL);
	}
	if(size)
		*size = 16;
	return true;
}


uint8_t *__write_bool(bool _bool, int *size)
{
	uint8_t *data = malloc(1);
	data[0] = _bool;
	if(size)
		*size = 1;
	return data;
}
uint8_t *__write_byte(int8_t _byte, int *size)
{
	uint8_t *data = malloc(1);
	data[0] = _byte;
	if(size)
		*size = 1;
	return data;
}
uint8_t *__write_unsigned_byte(uint8_t _unsigned_byte, int *size)
{
	uint8_t *data = malloc(1);
	data[0] = _unsigned_byte;
	if(size)
		*size = 1;
	return data;
}
uint8_t *__write_short(int16_t _short, int *size)
{
	uint8_t *data = malloc(2);
	data[0] = (_short & 0xff00) >> 8;
	data[1] = (_short & 0xff);
	if(size)
		*size = 2;
	return data;
}
uint8_t *__write_unsigned_short(uint16_t _unsigned_short, int *size)
{
	uint8_t *data = malloc(2);
	data[0] = (_unsigned_short & 0xff00) >> 8;
	data[1] = (_unsigned_short & 0xff);
	if(size)
		*size = 2;
	return data;
}
uint8_t *__write_int(int32_t _int, int *size)
{
	uint8_t *data = malloc(4);
	data[0] = (_int & 0xff000000) >> 24;
	data[1] = (_int & 0xff0000) >> 16;
	data[2] = (_int & 0xff00) >> 8;
	data[3] = (_int & 0xff);
	if(size)
		*size = 4;
	return data;
}
uint8_t *__write_long(int64_t _long, int *size)
{
	uint8_t *data = malloc(8);
	data[0] = (_long & 0xff00000000000000) >> 56;
	data[1] = (_long & 0xff000000000000) >> 48;
	data[2] = (_long & 0xff0000000000) >> 40;
	data[3] = (_long & 0xff00000000) >> 32;
	data[4] = (_long & 0xff000000) >> 24;
	data[5] = (_long & 0xff0000) >> 16;
	data[6] = (_long & 0xff00) >> 8;
	data[7] = (_long & 0xff);
	if(size)
		*size = 8;
	return data;
}
uint8_t *__write_float(float _float, int *size)
{
	int32_t holder = *((int32_t *) &_float);
	return __write_int(holder, size);
}
uint8_t *__write_double(double _double, int *size)
{
	int64_t holder = *((int64_t *) &_double);
	return __write_long(holder, size);
}
uint8_t *__write_string(const char *_string, int *size)
{
	size_t str_size = strlen(_string);
	int varint_buffer_size;
	int total_size;
	uint8_t *varint = __write_varint(str_size, &varint_buffer_size);
	int i;
	int j = 0;
	uint8_t *str_data = malloc(str_size);
	uint8_t *total_data;


	total_size = varint_buffer_size + str_size;
	total_data = malloc(total_size);


	for(i = 0; i < str_size; i++)
	{
		str_data[i] = _string[i];
	}

	memcpy(total_data, varint, varint_buffer_size);
	memcpy(total_data + varint_buffer_size, str_data, str_size);
	free(varint);
	free(str_data);

	if(size)
		*size = total_size;

	return total_data;

}
uint8_t *__write_varint(int32_t _varint, int *size)
{
	int data_size = 0;
	uint8_t *data = malloc(data_size);
	int i = 0;
	while(true)
	{
		if(i == 5)
		{
			return false;
		}
		if((_varint & ~0x7f) == 0)
		{
			data = realloc(data, ++data_size);
			data[data_size - 1] = _varint;
			break;
		}

		data = realloc(data, ++data_size);
		data[data_size - 1] = (_varint & 0x7f) | 0x80;
		_varint >>= 7;
		i++;
	}
	if(size)
		*size = data_size;
	return data;
}
uint8_t *__write_varlong(int64_t _varlong, int *size)
{
	int data_size = 0;
	uint8_t *data = malloc(data_size);
	int i = 0;
	while(true)
	{
		if(i == 10)
		{
			return false;
		}
		if((_varlong & ~0x7f) == 0)
		{
			data = realloc(data, ++data_size);
			data[data_size - 1] = _varlong;
			break;
		}

		data = realloc(data, ++data_size);
		data[data_size - 1] = (_varlong & 0x7f) | 0x80;
		_varlong >>= 7;
		i++;
	}
	if(size)
		*size = data_size;
	return data;
}
uint8_t *__write_position(position _position, int *size)
{
	int64_t	int_pos = 0;
	_position.x &= 0x3ffffff;
	_position.z &= 0x3ffffff;
	_position.y &= 0x1fff;

	int_pos |= (int64_t) _position.x << (64 - 26);
	int_pos |= (int64_t) _position.z << (64 - 26 - 26);
	int_pos |= (int64_t) _position.y;

	return __write_long(int_pos, size);
}
uint8_t *__write_angle(int8_t _angle, int *size)
{
	uint8_t *data = malloc(1);
	data[0] = _angle;
	if(size)
		*size = 1;
	return data;
}
uint8_t *__write_uuid(uuid uuid, int *size)
{
	uint8_t *data = malloc(16);

	data[0] = uuid[1] & 0xff00000000000000 >> 56;
	data[1] = uuid[1] & 0xff000000000000 >> 48;
	data[2] = uuid[1] & 0xff0000000000 >> 40;
	data[3] = uuid[1] & 0xff00000000 >> 32;
	data[4] = uuid[1] & 0xff000000 >> 24;
	data[5] = uuid[1] & 0xff0000 >> 16;
	data[6] = uuid[1] & 0xff00 >> 8;
	data[7] = uuid[1] & 0xff;
	data[8] = uuid[0] & 0xff00000000000000 >> 56;
	data[9] = uuid[0] & 0xff000000000000 >> 48;
	data[10] = uuid[0] & 0xff0000000000 >> 40;
	data[11] = uuid[0] & 0xff00000000 >> 32;
	data[12] = uuid[0] & 0xff000000 >> 24;
	data[13] = uuid[0] & 0xff0000 >> 16;
	data[14] = uuid[0] & 0xff00 >> 8;
	data[15] = uuid[0] & 0xff;
	if(size)
		*size = 16;
	return data;
}


bool should_wraparound(uint8_t *data, int data_size, struct wraparound *cutoff)
{
	int32_t length;
	int32_t length_size;

	__read_varint(data, data_size, &length, &length_size);

	if(data_size - length_size > length)
	{
		cutoff->cutoff = length_size + length;
		cutoff->first_size = length_size + length;
		cutoff->second_size = data_size - (length_size + length);
		return true;
	}
	return false;
}

void free_data_value(void *value)
{
	data_value *v = (data_value *) value;
	if(v->type == DTSTRING)
	{
		if(v->_string)
		{
			free(v->_string);
		}
	}
	if(v->type == DTBYTE_ARRAY)
	{
		free(v->_byte_array.data);
	}
	free(v);
}

void packet_destroy(struct packet *packet)
{
	map_destroy(packet->data);
	free(packet->name);
	free(packet);
}