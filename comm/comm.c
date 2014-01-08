#include <stdio.h>
#include <stdarg.h>
#include "comm.h"

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
        case SCAN:
            break;
        case MISSION:
            switch(cmd.order):
            {
                case START:
                case RESUME:
                    if (Running.running_thread)
                    pthread
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
