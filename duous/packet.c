#define _CRT_SECURE_NO_WARNINGS
#include "packet.h"
#include "logger.h"
#include "client.h"
#include <string.h>
/*

	first read the length.
	then read the id.
	use the id to read all the fields.
	uint8_t *data_pointer: an offset from data to read from.

*/


struct form forms[NUM_FORMS] = {
	{
		"Handshake",
		HANDSHAKING,
		0,
		4,
		{
			{
				DTVARINT,
				"Protocol Version"
			},
			{
				DTSTRING,
				"Server Address"
			},
			{
				DTUNSIGNED_SHORT,
				"Server Port"
			},
			{
				DTVARINT,
				"Next State"
			}
		}
	},
	{
		"Request",
		STATUS,
		0,
		0,
		{
			{ (data_type) NULL, NULL }
		}
	}
};

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
	[DTVARINT] = (data_read_fn_pointer) __read_varint,
	[DTSTRING] = (data_read_fn_pointer) __read_string,
	[DTUNSIGNED_SHORT] = (data_read_fn_pointer) __read_unsigned_short,
};

#define READ(type, dest)                                                                                  \
    do                                                                                                    \
    {                                                                                                     \
        int read_size;                                                                                    \
        bool r = read_pointers[type](data_pointer, data_size - (data_pointer - data), &dest, &read_size); \
        if (!r)                                                                                           \
        {                                                                                                 \
            return false;                                                                                 \
        }                                                                                                 \
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
	int64_t _uuid[2];

	packet->data = map_create(free_data_value);

	READ(DTVARINT, length);
	READ(DTVARINT, id);

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
				break;
			case DTBYTE:
				break;
			case DTUNSIGNED_BYTE:
				break;
			case DTSHORT:
				break;
			case DTUNSIGNED_SHORT:
				SETPACKETDATA(DTUNSIGNED_SHORT, _unsigned_short);
				break;
			case DTINT:
				break;
			case DTLONG:
				break;
			case DTFLOAT:
				break;
			case DTDOUBLE:
				break;
			case DTSTRING:
				SETPACKETDATA(DTSTRING, _string);
				break;
			case DTVARINT:
				SETPACKETDATA(DTVARINT, _varint);
				break;
			case DTVARLONG:
				break;
			case DTPOSITION:
				break;
			case DTANGLE:
				break;
			case DTUUID:
				break;
			default:
				break;
		}
		map_set(packet->data, form->fields[i].name, v);
	}
	packet->name = malloc(sizeof(*packet->name) * strlen(form->packet_name) + 1);
	packet->direction = SERVERBOUND;
	packet->id = id;
	strcpy(packet->name, form->packet_name);
	return true;
}
#undef READ

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
	{
		*varint = decoded_int;
	}
	*size = len;
	return true;
}

bool __read_string(uint8_t *data, int read_max, char **string, int *size)
{
	int32_t str_length;
	int i;
	char *str;
	int32_t varint_length;
	__read_varint(data, read_max, &str_length, &varint_length);

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
	*size = i + varint_length;
	return true;
}

bool __read_unsigned_short(uint8_t *data, int read_max, uint16_t *_unsigned_short, int *size)
{
	if(read_max < 2)
	{
		return false;
	}
	*_unsigned_short = 0;
	*_unsigned_short = (data[0] << 8) | data[1];
	*size = 2;
	return true;
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
	free(v);
}

void packet_destroy(struct packet *packet)
{
	map_destroy(packet->data);
	free(packet);
}