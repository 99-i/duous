#pragma once
#include <stdbool.h>
#include "types.h"

typedef enum biome_e
{
	BIOME_OCEAN,
	BIOME_PLAINS,
	BIOME_DESERT,
	BIOME_EXTREME_HILLS,
	BIOME_FOREST,
	BIOME_TAIGA,
	BIOME_SWAMPLAND,
	BIOME_RIVER,
	BIOME_HELL,
	BIOME_SKY,
	BIOME_FROZEN_OCEAN,
	BIOME_FROZEN_RIVER,
	BIOME_ICE_PLAINS,
	BIOME_ICE_MOUNTAINS,
	BIOME_MUSHROOM_ISLAND,
	BIOME_MUSHROOM_ISLAND_SHORE,
	BIOME_BEACH,
	BIOME_DESERT_HILLS,
	BIOME_FOREST_HILLS,
	BIOME_TAIGA_HILLS,
	BIOME_EXTREME_HILLS_EDGE,
	BIOME_JUNGLE,
	BIOME_JUNGLE_HILLS
} biome;



//16*16*16 blocks
struct chunk
{
	int block_ids[16][16][16];
};

struct world
{
	bool hardcore;
	bool generate_structures;
	bool raining;
	bool thundering;
	int rain_timer;
	int thunder_timer;
	position spawn_pos;
	long seed;
	long time;


};