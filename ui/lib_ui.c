#include <stdio.h>
#include <string.h>
#include "mongoose.h"

// This function will be called by mongoose on every new request
/*
static int index_html(struct mg_connection *conn)
{
    printf("%s\n",conn->uri);
    return 0;
}

*/

int run_server(void)
{
    struct mg_server *server;
    
    // Create and configure the server
    server = mg_create_server(NULL);
    mg_set_option(server,"listening_port","80");
    mg_set_option(server,"document_root","./web");
    //mg_add_uri_handler(server,"/",index_html);

    // Serve request. Hit Ctrl-C to terminate the program
    printf("Starting on port %s\n",mg_get_option(server,"listening_port"));
    while(1)
        mg_poll_server(server,1000);

    // Cleanup,and free server instance
    mg_destroy_server(&server);
    return 0;
}
