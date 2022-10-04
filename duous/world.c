#include "world.h"
#include "util.h"
#include "cJSON.h"
#include <stdlib.h>


/*json*/
struct level level_read(const char *path)
{
	struct level level;
	char *file = slurp_file_to_str(path);

	cJSON *main = cJSON_Parse(file);
	free(file);

	cJSON *hardcore = cJSON_GetObjectItem(main, "hardcore");
	cJSON *generate_structures = cJSON_GetObjectItem(main, "generate_structures");
	cJSON *raining = cJSON_GetObjectItem(main, "raining");
	cJSON *thundering = cJSON_GetObjectItem(main, "thundering");
	cJSON *rain_timer = cJSON_GetObjectItem(main, "rain_timer");
	cJSON *thunder_time = cJSON_GetObjectItem(main, "thunder_timer");

	cJSON *spawn_pos = cJSON_GetObjectItem(main, "spawn_pos");

	cJSON *spawn_pos_x = cJSON_GetObjectItem(spawn_pos, "x");
	cJSON *spawn_pos_y = cJSON_GetObjectItem(spawn_pos, "y");
	cJSON *spawn_pos_z = cJSON_GetObjectItem(spawn_pos, "z");

	level.hardcore = cJSON_IsTrue(hardcore);
	level.generate_structures = cJSON_IsTrue(generate_structures);
	level.raining = cJSON_IsTrue(raining);
	level.thundering = cJSON_IsTrue(thundering);
	level.rain_timer = cJSON_GetNumberValue(rain_timer);
	level.thunder_timer = cJSON_GetNumberValue(thunder_time);
	level.spawn_pos.x = cJSON_GetNumberValue(spawn_pos_x);
	level.spawn_pos.y = cJSON_GetNumberValue(spawn_pos_y);
	level.spawn_pos.z = cJSON_GetNumberValue(spawn_pos_z);

	return level;
}


struct world *world_create(void)
{
	struct world *world = malloc(sizeof(*world));
	world->level = level_read("worlds/default.json");
	world->num_regions = 0;
	world->regions = malloc(0);

	return world;
}

struct world *world_read(const char *path)
{
	FILE *file = fopen(path, "rb");
	struct world *world = malloc(sizeof(*world));
	int i;
	int j;

	world->level = level_read(file);
	fread(&world->num_regions, sizeof(int), 1, file);

	world->regions = malloc(sizeof(*world->regions));

	for(i = 0; i < world->num_regions; i++)
	{
		fread(&world->regions[i].x, sizeof(int), 1, file);
		fread(&world->regions[i].y, sizeof(int), 1, file);
		fread(&world->regions[i].num_chunks, sizeof(int), 1, file);

		for(j = 0; j < world->regions[i].num_chunks; j++)
		{
			fread(&world->regions[i].chunks[j], sizeof(struct chunk), 1, file);
		}

	}

	fclose(file);
	return world;

}

bool world_load_chunk(position chunk_coords)
{
	//first, determine if the region that contains the chunk is loaded.
	//if not, load it.
	//next, determine the level of the chunk.
	//third, generate the chunk.
	//TODO: chunk generation methods.


}

void world_write(const char *path);
