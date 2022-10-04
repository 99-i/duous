#include "util.h"
#include <stdio.h>
#include <stdlib.h>

char *slurp_file_to_str(const char *path)
{
	FILE *fp = fopen(path, "rb");
	long file_size;
	char *str;
	if(!fp)
	{
		return 0;
	}

	fseek(fp, 0, SEEK_END);

	file_size = ftell(fp);

	if(file_size == -1)
	{
		return 0;
	}

	str = malloc(file_size + 1);

	fseek(fp, 0, SEEK_SET);

	if(!fread(str, sizeof(char), file_size, fp))
	{
		free(str);
		return 0;
	}

	str[file_size] = 0;
	fclose(fp);

	return str;
}