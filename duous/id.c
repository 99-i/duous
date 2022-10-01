#include "id.h"
#include <stdint.h>
#include <stdlib.h>

int current_id = 0;

int id(void)
{
    return ++current_id;
}