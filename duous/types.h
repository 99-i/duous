#pragma once
#include <stdint.h>

typedef struct position_s
{
	int32_t x;
	int32_t y;
	int32_t z;
} position;

typedef struct look_s
{
	int8_t yaw;
	int8_t pitch;
} look;
