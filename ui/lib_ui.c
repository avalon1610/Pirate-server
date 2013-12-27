#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define bool int
#define true 1
#define false 0

#include "cJSON.h"
#include "mongoose.h"
#include "comm.h"

extern LIST_ENTRY mission_list;
extern pthread_rwlock_t rwlock;

static int parse_data(const char *data)
{
    char *param;
    cJSON *root = cJSON_Parse(data);
    cJSON *type_entry;
    MISSION *mission = (MISSION *)malloc(sizeof(MISSION));
    memset(mission,0,sizeof(MISSION));

    type_entry = cJSON_GetObjectItem(root,"config-type");
    mission->type = (MISSION_TYPE)(type_entry->valueint);
    mission->param = root;

    pthread_rwlock_wrlock(&rwlock);
    InsertTailList(&mission_list,&mission->node);
    pthread_rwlock_unlock(&rwlock);
}

static int config_setup(struct mg_connection *conn)
{
    static const char *reply = "HTTP/1.0 200 OK\r\n"
        "Access-Control-Allow-Origin:http://127.0.0.1\r\n\r\n"
        "Hello!";
    parse_data(conn->query_string);

    mg_write(conn,reply,strlen(reply));
    return 0;
}

int run_server(void)
{
    struct mg_server *server;
    
    // Create and configure the server
    server = mg_create_server(NULL);
    mg_set_option(server,"listening_port","80");
    mg_set_option(server,"document_root","./web");
    mg_add_uri_handler(server,"/config",config_setup);

    // Serve request. Hit Ctrl-C to terminate the program
    printf("Starting on port %s\n",mg_get_option(server,"listening_port"));
    while(1)
        mg_poll_server(server,1000);

    // Cleanup,and free server instance
    mg_destroy_server(&server);
    return 0;
}
