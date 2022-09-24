

#include "application.h"
#include "main.h"
#include "uv.h"
#include "console.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

struct application *app;


int main(void)
{
    srand(time(NULL));
    app = application_create();
    application_add_toplevel_server(app, server_create(25565));
    application_run(app);
    return 0;
}
