#pragma once
#include <stdint.h>

typedef struct position_s
{
	int x;
	int y;
	int z;
} position;

typedef struct vector3_s
{
	float x;
	float y;
	float z;
} vector3;


typedef struct look_s
{
	int8_t yaw;
	int8_t pitch;
} look;
