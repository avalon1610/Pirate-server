#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "cJSON.h"
#include "mongoose.h"
#include "comm.h"
#include "network.h"

extern LIST_ENTRY mission_list;
extern pthread_rwlock_t rwlock;
extern pthread_rwlock_t rwlock_env;
extern ENV *env;
extern RUNNING_MISSION *Running;

int command_control(COMMAND cmd)
{
    switch(cmd.type)
    {
        case T_SCAN:
            break;
        case T_MISSION:
            switch(cmd.order)
            {
                case START:
                case RESUME:
                    if (Running->running_thread_id == 0) 
                        pthread_create(NULL,NULL,(void *)Test_Work,&cmd);
                    break;
                case PAUSE:
                case STOP:
                    
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

static int parse_mission(const char *data,char *msg)
{
    char *param;
    cJSON *root;
    cJSON *type_entry;
    MISSION *mission; 

    if (data == NULL)
        return false;

    mission = (MISSION *)malloc(sizeof(MISSION));
    if (mission == NULL)
    {
        strcpy(msg,"Insufficient Memory");
        return false;
    }
    memset(mission,0,sizeof(MISSION));

    root = cJSON_Parse(data);
    type_entry = cJSON_GetObjectItem(root,"mission-type");
    if (type_entry == NULL)
    {
        strcpy(msg,"Invalid Parameter");
        return false;
    }

    mission->type = (MISSION_TYPE)(type_entry->valueint);
    mission->param = root;

    pthread_rwlock_wrlock(&rwlock);
    InsertTailList(&mission_list,&mission->node);
    pthread_rwlock_unlock(&rwlock);

    DbgPrint("Add Mission:%d\n",mission->type);
    return true;
}

static char *get_post_data(struct mg_connection *conn)
{
    char *data;
    if (conn->content_len == 0)
        return NULL;

    data = (char *)malloc(conn->content_len);
    memset(data,0,conn->content_len);
    memcpy(data,conn->content,conn->content_len);
    return data;
}

typedef int(*SETUP_FUNCTION)(const char *,char *);

static void handle_request(struct mg_connection *conn,SETUP_FUNCTION func)
{
    char *reply_header = "HTTP/1.0 200 OK\r\n"
        "Access-Control-Allow-Origin:";
    char reply[256] = {0};
    char error_msg[32] = {0};
    char *post_data = get_post_data(conn);
    char *origin = mg_get_header(conn,"Origin");
    if (func == NULL)
        return;

    if (func(post_data,error_msg))
        sprintf(reply,"%s%s\r\n\r\nOK",reply_header,origin);
    else
        sprintf(reply,"%s%s\r\n\r\n%s",reply_header,origin,error_msg);

    mg_write(conn,reply,strlen(reply));
    free(post_data);
}

static int mission_setup(struct mg_connection *conn)
{
    handle_request(conn,parse_mission);
    return 0;
}

static int parse_env(const char *data,char *msg)
{
    cJSON *root;
    char *temp_str;
    cJSON *temp_obj_1;
    cJSON *temp_obj_2;
    bool ret = false;

    if (!env)
        return ret;

    root = cJSON_Parse(data);
    temp_str = cJSON_GetObjectItem(root,"host")->valuestring;
    strcpy(env->host,temp_str);
    temp_obj_1 = cJSON_GetObjectItem(root,"target1");
    temp_obj_2 = cJSON_GetObjectItem(root,"target2");
    if (temp_obj_1 == NULL && temp_obj_2 == NULL)
    {
        strcpy(msg,"Invalid Parameter");
        goto Cleanup;
    }

    pthread_rwlock_wrlock(&rwlock_env);
    if (temp_obj_1)
        strcpy(env->target1,temp_obj_1->valuestring);
    if (temp_obj_2)
        strcpy(env->target2,temp_obj_2->valuestring);
    pthread_rwlock_unlock(&rwlock_env);
    ret = true;

    DbgPrint("Receive Env:\n host:%s\n target1:%s\n target2:%s\n",
           env->host,
           env->target1?env->target1:(unsigned char *)"NULL",
           env->target2?env->target2:(unsigned char *)"NULL");
Cleanup:
    cJSON_Delete(root);
    return ret;
}

static int parse_runner(const char *data,char *msg)
{
    cJSON *root;
    int status;
    bool ret = false;
    int type;
    LIST_ENTRY *current = mission_list.Flink;
    MISSION *entry;
    if (data == NULL)
        return ret;

    root = cJSON_Parse(data);
    status = cJSON_GetObjectItem(root,"status")->valueint;
    type = cJSON_GetObjectItem(root,"type")->valueint;

    pthread_rwlock_wrlock(&rwlock);
    while (current != &mission_list)
    {
        entry = CONTAINING_RECORD(current,MISSION,node);
        if (entry->type == type)
        {
            entry->status = status;
            ret = true;
            break;
        }
        current = current->Flink;
    }
    pthread_rwlock_unlock(&rwlock);

    if (ret)
    {
        COMMAND cmd;
        memset(&cmd,0,sizeof(cmd));
        cmd.type = T_MISSION;
        cmd.order = status;
        cmd.m_type = type;
        command_control(cmd);
    }
    else
        strcpy(msg,"Invalid Parameter");

Cleanup:
    cJSON_Delete(root);
    return ret;
}

static int parse_scan(const char *data,char *msg)
{
}

static int evn_setup(struct mg_connection *conn)
{
    handle_request(conn,parse_env);
    return 0;
}


static int runner_setup(struct mg_connection *conn)
{
    handle_request(conn,parse_runner);
    return 0;
}


static int scan_setup(struct mg_connection *conn)
{
    handle_request(conn,parse_scan);
    return 0;
}

int run_server(void)
{	
    struct mg_server *server;
    
    // Create and configure the server
    server = mg_create_server(NULL);
    mg_set_option(server,"listening_port","80");
    mg_set_option(server,"document_root","./web");
    mg_add_uri_handler(server,"/mission",mission_setup);
    mg_add_uri_handler(server,"/env",evn_setup);
    mg_add_uri_handler(server,"/scan",scan_setup);
    mg_add_uri_handler(server,"/runner",runner_setup);

    // Serve request. Hit Ctrl-C to terminate the program
    printf("Starting on port %s\n",mg_get_option(server,"listening_port"));
    while(1)
        mg_poll_server(server,1000);

    // Cleanup,and free server instance
    mg_destroy_server(&server);
    return 0;
}
