#pragma once
#include "map.h"
#include "stdint.h"
#include "client.h"

typedef enum packet_direction_e
{
	SERVERBOUND,
	CLIENTBOUND
} packet_direction;
const char *packet_direction_str(packet_direction pd);

typedef enum data_type_e
{
	DTBOOL,
	DTBYTE,
	DTUNSIGNED_BYTE,
	DTSHORT,
	DTUNSIGNED_SHORT,
	DTINT,
	DTLONG,
	DTFLOAT,
	DTDOUBLE,
	DTSTRING,
	DTVARINT,
	DTVARLONG,
	/*ENTITY_METADATA,*/
	/*SLOT,*/
	/*NBT_TAG,*/
	DTPOSITION,
	DTANGLE,
	DTUUID,
	/*OPTIONAL_X,*/
	/*ARRAY_OF_X,*/
	/*X_ENUM,*/
	/*BYTE_ARRAY,*/
	NUM_DATA_TYPES
} data_type;

typedef struct position_s
{
	int32_t x;
	int32_t y;
	int32_t z;
} position;

typedef uint64_t uuid[2];
typedef struct data_value_s
{
	data_type type;
	union
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

		/* hi then lo */
		uuid _uuid;
	};
} data_value;
void free_data_value(void *value);
struct packet
{
	packet_direction direction;
	int id;
	char *name;
	map data;
};

struct form
{
	const char *packet_name;
	client_state state;
	int id;
	int num_fields;
	struct field
	{
		data_type type;
		const char *name;
	} fields[4];
};

#define NUM_FORMS 2
extern struct form forms[NUM_FORMS];

struct wraparound
{
	int32_t cutoff;
	int32_t first_size;
	int32_t second_size;
};

bool packet_read(struct packet *packet, uint8_t *data, int data_size, client_state state);

bool packet_write(struct packet *packet, uint8_t **data);
/*  read_max: the max amount of bytes that can be read theoretically without a memory access exception
	size: a pointer to an int that is filled with the amount of bytes read
*/
bool __read_bool(uint8_t *data, int read_max, bool *_bool, int *size);
bool __read_byte(uint8_t *data, int read_max, int8_t *_byte, int *size);
bool __read_unsigned_byte(uint8_t *data, int read_max, uint8_t *_unsigned_byte, int *size);
bool __read_short(uint8_t *data, int read_max, int16_t *_short, int *size);
bool __read_unsigned_short(uint8_t *data, int read_max, uint16_t *_unsigned_short, int *size);
bool __read_int(uint8_t *data, int read_max, int32_t *_int, int *size);
bool __read_long(uint8_t *data, int read_max, int64_t *_long, int *size);
bool __read_float(uint8_t *data, int read_max, float *_float, int *size);
bool __read_double(uint8_t *data, int read_max, double *_double, int *size);
bool __read_string(uint8_t *data, int read_max, char **_string, int *size);
bool __read_varint(uint8_t *data, int read_max, int32_t *_varint, int *size);
bool __read_varlong(uint8_t *data, int read_max, int64_t *_varlong, int *size);
bool __read_position(uint8_t *data, int read_max, position *_position, int *size);
bool __read_angle(uint8_t *data, int read_max, int8_t *_angle, int *size);
bool __read_uuid(uint8_t *data, int read_max, uuid *uuid, int *size);


uint8_t *__write_bool(bool _bool, int *size);
uint8_t *__write_byte(int8_t _byte, int *size);
uint8_t *__write_unsigned_byte(uint8_t _unsigned_byte, int *size);
uint8_t *__write_short(int16_t _short, int *size);
uint8_t *__write_unsigned_short(uint16_t _unsigned_short, int *size);
uint8_t *__write_int(int32_t _int, int *size);
uint8_t *__write_long(int64_t _long, int *size);
uint8_t *__write_float(float _float, int *size);
uint8_t *__write_double(double _double, int *size);
uint8_t *__write_string(const char *_string, int *size);
uint8_t *__write_varint(int32_t _varint, int *size);
uint8_t *__write_varlong(int64_t _varlong, int *size);
uint8_t *__write_position(position _position, int *size);
uint8_t *__write_angle(int8_t _angle, int *size);
uint8_t *__write_uuid(uuid uuid, int *size);


bool should_wraparound(uint8_t *data, int data_size, struct wraparound *cutoff);

void packet_destroy(struct packet *packet);