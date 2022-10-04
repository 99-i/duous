#pragma once
#include "types.h"
#include <stdio.h>
#include <stdbool.h>

enum biome
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
};

enum dimension
{
	DIMENSION_NETHER = -1,
	DIMENSION_OVERWORLD,
	DIMENSION_END,

};


//level: a number between 0 and 31, 0 being the highest
//	and 31 being the lowest chunk level (in terms of y coordinate).
struct chunk
{
	int level;
	uint8_t biomes[16][16];
	char blocks[16][16][16];
};


//each region is a column of up to 32 chunks (512 block limit).
struct region
{
	int x;
	int y;
	int num_chunks;
	struct chunk *chunks;
};

struct level
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

//in memory representation of a world.
//the regions array is an array of the loaded regions of the world.
struct world
{
	struct level level;
	int num_regions;
	struct region *regions;
};


struct world *world_create(void);
struct world *world_read(const char *path);

bool world_load_chunk(position chunk_coords);

void world_write(const char *path);


