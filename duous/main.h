#pragma once

#define COMMAND_BUFFER_SIZE 256
struct server *server;
void run_repl(void *arg);
extern char command[COMMAND_BUFFER_SIZE];
