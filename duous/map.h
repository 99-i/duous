#pragma once
#include <stdbool.h>

/* a map that points c strings to void * pointers */
typedef struct map_s
{
    struct map_data_entry
    {
        char *key;
        void *value;
    } * data;
    int size;
    void (*free_function)(void *);
} map_t;

typedef map_t *map;

map map_create(void (*free_function)(void *));

void map_set(map m, const char *key, void *v);

void *map_get(map m, const char *key, bool *found);

void map_unset(map m, const char *key);

void map_destroy(map m);

int map_size(map m);
void *map_getvi(map m, int i);

char *map_getki(map m, int i);