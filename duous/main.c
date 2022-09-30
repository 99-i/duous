

#include "server.h"
#include "main.h"
#include "uv.h"
#include "console.h"
#include "logger.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

struct application *app;
char command[COMMAND_BUFFER_SIZE] = {0};

int main(void)
{
	srand(time(NULL));

	server = server_create(25565);

	uv_thread_t *repl_thread = malloc(sizeof(*repl_thread));
	int r = uv_thread_create(repl_thread, run_repl, 0);


	server_run(server);
	uv_thread_join(repl_thread);


	return 0;
}

void run_repl(void *arg)
{
	char c;
	int i;
	while(!server->running);;
	uv_sleep(1);
	printf(">");
	while(c = _getch())
	{
		switch(c)
		{
			case '\r':
				printf("\n");
				if(!strcmp(command, "reload") || !strcmp(command, "rl"))
				{
					game_restart_lua(server->game);
				}
				else if(!strcmp(command, "clear"))
				{
					clear();
				}
				else
				{
					LOGFMT(("Command"), ("Unknown command."));
				}
				for(i = 0; i < COMMAND_BUFFER_SIZE; i++)
				{
					command[i] = 0;
				}
				setcaret();
				break;
			case '\b':
				if(strlen(command) > 0)
				{
					command[strlen(command) - 1] = 0;
				}
				break;
			default:
				command[strlen(command)] = c;
				break;
		}

		if(c != '\r')
		{
			setcaret();
		}
	}
}
