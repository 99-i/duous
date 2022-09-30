#define _CRT_SECURE_NO_WARNINGS
#include "logger.h"
#include "console.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void _log(const char *tag, const char *message)
{
	deletecaret();
	time_t now;
	time(&now);
	setcolor(GREEN);
	printf("%.19s ", ctime(&now));
	setcolor(GREY);
	printf("[%s]: ", tag);
	setcolor(BLUE);
	printf("%s\n", message);
	setcolor(WHITE);
	setcaret();
}