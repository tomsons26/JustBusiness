//temp
#include "mapview.h"

#include <cstdio>

void Prog_End();

void Exit(int errorval, const char *message, ...)
{
    char errstring[256];
    va_list argptr;

    va_start(argptr, message);
    Prog_End();
    if (message) {
        if (*message) {
            vsprintf(errstring, message, argptr);
            printf(errstring);
        }
    }
    exit(errorval);
}

void Prog_End()
{
}