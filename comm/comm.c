#include <stdio.h>
#include <stdarg.h>

int DbgPrint(const char *format,...)
{
//#ifdef DEBUG
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
//#endif
    return 0;
}
