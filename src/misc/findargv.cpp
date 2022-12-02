//temp

#include <cstring>
#include <stdlib.h>

char *Find_Argv(char const *str)
{
    //not sure
#if 0
    int min = strlen(str);
    for (int i = 0; i < __dll_argc; ++i) {
        int len = strlen(*((const char **)__dll_argv + i));

        if (min <= len && !strnicmp(*((const char **)__dll_argv + i), str, len)) {
            return (char *)*((_DWORD *)__dll_argv + i);
        }
    }
#endif
    int min = strlen(str);
    for (int i = 0; i < __argc; ++i) {
        int len = strlen(*((const char **)__argv + i));

        if (min <= len && !strnicmp(*((const char **)__argv + i), str, len)) {
            return (char *)*((int *)__argv + i);
        }
    }
    return nullptr;
}
