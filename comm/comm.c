

int DbgPrint(const char *format,...)
{
#ifdef Debug
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
    return 0;
}
