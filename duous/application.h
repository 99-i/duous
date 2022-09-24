#pragma once
#include "server.h"

struct application
{
    int id;

    int num_servers;
    struct server **servers;

    uv_loop_t loop;
    uv_tcp_t proxy;

    int num_threads;
    uv_thread_t **threads;
};

/* create an application object. */
struct application *application_create(void);

/* add an existing server to the toplevel server list. */
void application_add_toplevel_server(struct application *application, struct server *server);

/* runs the application, which means running all of the servers. */
void application_run(struct application *application);

void application_add_thread(struct application *application, uv_thread_t *thread);