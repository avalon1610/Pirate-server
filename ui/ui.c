#include "comm.h"
#include "mongoose.h"
#include "network.h"
#include "zlog.h"
#include "monitor.h"

extern LIST_ENTRY mission_list;
extern pthread_rwlock_t rwlock;
extern pthread_rwlock_t rwlock_env;
extern ENV *env;
extern RUNNING_MISSION *Running;
extern zlog_category_t *c;

LIST_ENTRY feedback_list;
pthread_rwlock_t fblock = PTHREAD_RWLOCK_INITIALIZER;

typedef struct tagFEEDBACK
{
    LIST_ENTRY node;
    int seq;
    char triptime[32];
    char error[64];
} FEEDBACK,*PFEEDBACK;

void Feedback(long seq,long triptime,char *error)
{
    FEEDBACK *fb = (FEEDBACK *)malloc(sizeof(FEEDBACK));
    memset(fb,0,sizeof(FEEDBACK));

    if (triptime == 0 && error == NULL)
    {
        zlog_error(c,"Monitor Feedback error occured!\n");
        return;
    }

    if (triptime != 0)
    {
        if (triptime >= 100000)
            sprintf(fb->triptime,"%ld",triptime/1000);
        else if (triptime >= 10000)
            sprintf(fb->triptime,"%ld.%01ld",triptime/1000,(triptime % 1000)/100);
        else if (triptime >= 1000)
            sprintf(fb->triptime,"%ld.%02ld",triptime/1000, (triptime % 1000)/10);
        else
            sprintf(fb->triptime,"%ld.%03ld",triptime/1000, triptime % 1000);
    }

    if (error != NULL)
    {
        strcpy(fb->error,error);
    }

    fb->seq = seq;

    // no '\n' in this zlog_debug
    zlog_debug(c,"Feedback seq:%d tt:%sms error:%s",seq,
                fb->triptime != 0 ? fb->triptime : "0",
                error ? error : "\n");

    // send feedback to list
    pthread_rwlock_wrlock(&fblock);
    InsertTailList(&feedback_list,&fb->node);
    pthread_rwlock_unlock(&fblock);
}

static int mission_ctl(MISSION_TYPE t,bool go)
{
    MISSION *entry;
    bool find = false;
    LIST_ENTRY *current = mission_list.Flink;
    pthread_rwlock_rdlock(&rwlock);
    while (current != &mission_list)
    {
        entry = CONTAINING_RECORD(current,MISSION,node);
        if (entry->type == t)
        {
            if (go)
                entry->status = RUNNING;
            else
                entry->status = STOPPED;
            find = true;
            break;
        }
        current = current->Flink;
    }
    pthread_rwlock_unlock(&rwlock);
    zlog_debug(c,"mission ctrl[%d]type[%d]: %s\n",go,t,find?"OK":"Failed");
    return find;
}

int command_control(COMMAND cmd)
{
	pthread_t  pid;
    int ret;
    if (Running == NULL)
        return false;
    switch(cmd.type)
    {
        case T_SCAN:
            break;
        case T_MISSION:
            switch(cmd.order)
            {
                int ret;
                case START:
                case RESUME:
                    mission_ctl(cmd.m_type,true);
                    ret = monitor_ctrl(M_START,M_PING);
                    if (M_ERROR == ret)
                    {
                        zlog_error(c,"monitor error when start!\n");
                        return false;
                    }
                    if (Running->running_thread_id == 0)
                        zlog_debug(c,"now running mission thread...\n");
                        //pthread_create(&pid,NULL,(void *)Test_Work,&cmd);
                    break;
                case PAUSE:
                case STOP:
                    mission_ctl(cmd.m_type,false);
                    ret = monitor_ctrl(M_STOP,M_PING);
                    if (M_ERROR == ret)
                    {
                        zlog_error(c,"monitor error when stop!\n");
                        return false;
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return true;
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

    zlog_debug(c,"Add Mission:%d\n",mission->type);
    return true;
}

static char *get_post_data(struct mg_connection *conn)
{
    char *data;
    if (conn->content_len == 0)
        return NULL;

    data = (char *)NEW(conn->content_len);
    memcpy(data,conn->content,conn->content_len);
    return data;
}

typedef int(*SETUP_FUNCTION)(const char *,char *);

static int handle_request(struct mg_connection *conn,SETUP_FUNCTION func)
{
    char reply[256] = {0};
    char error_msg[32] = {0};
    char *post_data;
    char *origin;

    post_data = get_post_data(conn);
    if (post_data == NULL)
    {
        strcpy(error_msg,"Invalid Parameter");
        goto EXIT;
    }
    origin = (char *)mg_get_header(conn,"Origin");
    if (origin == NULL)
    {
        zlog_error(c,"Origin is NULL\n");
        return;
    }

    if (func == NULL)
    {
        zlog_error(c,"callback function is NULL\n");
        return;
    }

    if (func(post_data,error_msg))
        strcpy(reply,"OK");
    else
        sprintf(reply,"%s",error_msg);
EXIT:
    DELETE(post_data);
    if (conn->is_websocket)
    {
        mg_websocket_write(conn,1,reply,strlen(reply));
        return false; //websocket should return 0 keep connection alive
    }
    else
    {
        mg_send_header(conn,"Access-Control-Allow-Origin",origin);
        mg_send_data(conn,reply,strlen(reply));
        return true;
    }
}

static int mission_setup(struct mg_connection *conn)
{
    return handle_request(conn,parse_mission);
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

    zlog_debug(c,"Receive Env:host[%s] target1[%s] target2[%s]\n",
           env->host,
           env->target1?env->target1:(unsigned char *)"NULL",
           env->target2?env->target2:(unsigned char *)"NULL");
Cleanup:
    cJSON_Delete(root);
    return ret;
}

// this callback should be websocket
static int parse_runner(const char *data,char *msg)
{
    cJSON *root;
    int order;
    bool ret = false;
    int type;
    int status;
    LIST_ENTRY *current = mission_list.Flink;
    MISSION *entry;

    if (data == NULL)
        return ret;

    root = cJSON_Parse(data);
    order = cJSON_GetObjectItem(root,"order")->valueint;
    type = cJSON_GetObjectItem(root,"type")->valueint;

    zlog_debug(c,"runner get order:%d,type:%d\n",order,type);
    pthread_rwlock_rdlock(&rwlock);
    while (current != &mission_list)
    {
        entry = CONTAINING_RECORD(current,MISSION,node);
        if (entry->type == type)
        {
            // find the mission
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
        cmd.order = order;
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
    return handle_request(conn,parse_env);
}


static int runner_setup(struct mg_connection *conn)
{
    return handle_request(conn,parse_runner);
}


static int scan_setup(struct mg_connection *conn)
{
    return handle_request(conn,parse_scan);
}

static int iterate_callback(struct mg_connection *conn)
{
    if (conn->is_websocket && !IsListEmpty(&feedback_list))
    {
        FEEDBACK *fb;
        cJSON *temp = NULL;
        char *ret;
        LIST_ENTRY *current;
        cJSON *result = cJSON_CreateArray();

        pthread_rwlock_wrlock(&fblock);
        while (!IsListEmpty(&feedback_list))
        {
            current = RemoveHeadList(&feedback_list);
            fb = CONTAINING_RECORD(current,FEEDBACK,node);
            cJSON_AddItemToArray(result,temp=cJSON_CreateObject());
            cJSON_AddNumberToObject(temp,"seq",fb->seq);
            cJSON_AddStringToObject(temp,"content",fb->triptime?fb->triptime:fb->error);
            if (fb)
            {
                free(fb);
                fb = NULL;
            }
        }
        pthread_rwlock_unlock(&fblock);

        ret = cJSON_Print(result);
        //zlog_debug(c,"ret:%s\n",ret);
        mg_websocket_write(conn,1,ret,strlen(ret));
        cJSON_Delete(result);
    }
    return 1;
}

// This handler is called for each incoming websocket frame, one or more
// times for connection lifetime.
enum SETUP_TYPE
{
    INIT = 0,
    OTHER
};

static void push_mission(struct mg_connection *conn)
{
    cJSON *temp;
    cJSON *result = NULL;
    MISSION *entry;
    char *ret = NULL;
    LIST_ENTRY *current = mission_list.Flink;
    pthread_rwlock_rdlock(&rwlock);
    while (current != &mission_list)
    {
        entry = CONTAINING_RECORD(current,MISSION,node);
        temp = cJSON_CreateBool(entry->type);
        cJSON_AddItemToObject(result,"mission_type",temp);
        current = current->Flink;
    }
    pthread_rwlock_unlock(&rwlock);

    if (result)
    {
        ret = cJSON_Print(result);
        mg_websocket_write(conn,1,ret,strlen(ret));
    }
    else
    {
        mg_websocket_write(conn,1,"0",1);
    }
    cJSON_Delete(result);
}

static int ws_handler(struct mg_connection *conn)
{
    static const char oops[] = "websocket data expected\n";
    int exit_flag = 0;// 0 means call again and again

    if (!conn->is_websocket)
    {
        zlog_error(c,"got unexpect data, not websocket\n");
        mg_send_data(conn,oops,strlen(oops));
        return 1;
    }

    char *buf = get_post_data(conn);
    if (buf == NULL)
        // may be a heartbeat 
        goto EXIT;

    zlog_debug(c,"websocket conn:%p\n",conn);
    zlog_debug(c,"setup websocket recv buf:%s\n",buf);
    cJSON *root = cJSON_Parse(buf);
    cJSON *temp = cJSON_GetObjectItem(root,"setup_type");
    if (temp == NULL)
    {
        zlog_error(c,"system initial setup parameter error\n");
        return 1;
    }

    int setup_type = temp->valueint;
    switch (setup_type)
    {
        case INIT:
            // push mission list to new client
            push_mission(conn);
            break;
        case OTHER:
            break;
        default:
            break;
    }

    if (conn->content_len == 4 && !memcmp(conn->content,"exit",4))
    {
        zlog_debug(c,"receive exit,abort.\n");
        exit_flag = 1;// not 0 means disconnect websocket
    }

EXIT:
    DELETE(buf);
    return exit_flag;
}

int run_server(void)
{
    struct mg_server *server;
    unsigned int current_timer = 0,last_timer = 0;
 
    // Create and configure the server
    server = mg_create_server(NULL);
    mg_set_option(server,"listening_port","80");
    mg_set_option(server,"document_root","./web");
    mg_add_uri_handler(server,"/mission",mission_setup);
    mg_add_uri_handler(server,"/env",evn_setup);
    mg_add_uri_handler(server,"/scan",scan_setup);
    mg_add_uri_handler(server,"/runner",runner_setup);
    mg_add_uri_handler(server,"/setup",ws_handler);

    InitializeListHead(&feedback_list);
    // Serve request. Hit Ctrl-C to terminate the program
    zlog_info(c,"Starting on port %s\n",mg_get_option(server,"listening_port"));
    while(1)
    {
        current_timer = mg_poll_server(server,1000);
        if (current_timer - last_timer > 1)
        {
            last_timer = current_timer;
            mg_iterate_over_connections(server,iterate_callback,NULL);
        }
    }

    // Cleanup,and free server instance
    mg_destroy_server(&server);
    return 0;
}
