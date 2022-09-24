#define _CRT_SECURE_NO_WARNINGS
#include "map.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

map map_create(void (*free_function)(void *))
{
    map m = malloc(sizeof(*m));
    m->data = 0;
    m->size = 0;
    m->free_function = free_function;
    return m;
}
struct map_data_entry *get_data_entry(map m, const char *key);

void map_set(map m, const char *key, void *v)
{
    struct map_data_entry *entry;
    int index;

    entry = get_data_entry(m, key);

    if (entry == NULL)
    {
        m->size++;
        m->data = realloc(m->data, sizeof(*(m->data)) * m->size);
        m->data[m->size - 1].key = malloc(strlen(key) + 1);
        strcpy(m->data[m->size - 1].key, key);
        m->data[m->size - 1].value = v;
    }
    else
    {
        index = entry - m->data;
        if (m->free_function != NULL)
        {
            m->free_function(m->data[index].value);
        }
        m->data[index].value = v;
    }
}

void *map_get(map m, const char *key, bool *found)
{
    struct map_data_entry *entry;

    entry = get_data_entry(m, key);

    if (entry == NULL)
    {
        *found = false;
        return (void *)-1;
    }
    return entry->value;
}

void map_unset(map m, const char *key)
{
    struct map_data_entry *entry;
    int index;
    int i;
    entry = get_data_entry(m, key);
    if (entry == NULL)
    {
        return;
    }

    index = entry - m->data;

    for (i = index; i < m->size - 1; i++)
    {
        m->data[i] = m->data[i + 1];
    }
    m->size--;
    m->data = realloc(m->data, sizeof(*m->data) * m->size);
}

void map_destroy(map m)
{
    int i;
    for (i = 0; i < m->size; i++)
    {
        free(m->data[i].key);
        if (m->free_function == NULL)
        {
            free(m->data[i].value);
        }
        else
        {
            m->free_function(m->data[i].value);
        }
    }
    free(m);
}

struct map_data_entry *get_data_entry(map m, const char *key)
{
    int i;
    for (i = 0; i < m->size; i++)
    {
        if (!strcmp(m->data[i].key, key))
        {
            return &m->data[i];
        }
    }
    return NULL;
}

int map_size(map m)
{
    return m->size;
}
void *map_getvi(map m, int i)
{
    return m->data[i].value;
}
char *map_getki(map m, int i)
{
    return m->data[i].key;
}