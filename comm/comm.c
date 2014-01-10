#include <stdio.h>
#include <stdarg.h>
#include "comm.h"
#include "network.h"

extern RUNNING_MISSION *Running;

int DbgPrint(const char *format,...)
{
#ifdef DEBUG
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
    return 0;
}

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
                        pthread_create(NULL,NULL,Test_Work,&cmd);
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
