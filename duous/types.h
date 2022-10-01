#pragma once
#include <stdint.h>

typedef struct position_s
{
	double x;
	double y;
	double z;
} position;

typedef struct look_s
{
	int8_t yaw;
	int8_t pitch;
} look;
