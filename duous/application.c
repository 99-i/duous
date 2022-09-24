#include "application.h"
#include "id.h"
#include "logger.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct application *application_create(void)
{
    struct application *app = malloc(sizeof(*app));
    app->id = id();
    app->num_servers = 0;
    uv_loop_init(&app->loop);
    uv_tcp_init(&app->loop, &app->proxy);
    app->servers = 0;
    app->threads = 0;
    app->num_threads = 0;

    return app;
}

void application_add_toplevel_server(struct application *application, struct server *server)
{
    application->num_servers++;
    application->servers = realloc(application->servers, sizeof(*application->servers) * application->num_servers);
    application->servers[application->num_servers - 1] = server;

    LOGFMT(APPTAG(), ("Propped server with ID %x.", server->id));
}

void application_run(struct application *application)
{
    int i;
    for (i = 0; i < application->num_servers; i++)
    {
        server_run(application->servers[i]);
    }

    for (i = 0; i < application->num_threads; i++)
    {
        uv_thread_join(application->threads[i]);
    }
}

void application_add_thread(struct application *application, uv_thread_t *thread)
{
    application->num_threads++;
    application->threads = realloc(application->threads, sizeof(*application->threads) * application->num_threads);
    application->threads[application->num_threads - 1] = thread;
}