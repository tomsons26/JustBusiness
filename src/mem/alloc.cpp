#include "wwmem.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>


long Total_Ram_Free(MemoryFlagType flag);
//temp

unsigned long MinRam = 0L;
unsigned long MaxRam = 0L;
static unsigned long TotalRam = 0L;
static unsigned long Memory_Calls = 0L;

void (*Memory_Error)(void) = NULL;


void *Alloc(unsigned long bytes_to_alloc, MemoryFlagType flags)
{
    unsigned char *retval = NULL;
    unsigned long original_size;
    unsigned long bytesfree;

    original_size = bytes_to_alloc;

    if (!TotalRam) {
        TotalRam = Total_Ram_Free(MEM_NORMAL);
    }

    retval = (unsigned char *)malloc(bytes_to_alloc);

    if (retval == NULL) {
        if (Memory_Error != NULL)
            Memory_Error();
        return NULL;
    }

    if (flags & MEM_CLEAR) {
        unsigned char *ptr;

        ptr = retval;
        memset(ptr, '\0', original_size);
    }

    bytesfree = Total_Ram_Free(MEM_NORMAL);
    if (bytesfree < MinRam) {
        MinRam = bytesfree;
    }
    if (TotalRam - bytesfree > MaxRam) {
        MaxRam = TotalRam - bytesfree;
    }

    Memory_Calls++;

    return (retval);
}

void Free(void const *pointer)
{
    if (pointer) {
        free((void *)pointer);
        Memory_Calls--;
    }

}

void *Resize_Alloc(void *original_ptr, unsigned long new_size_in_bytes)
{
    unsigned long *temp;

    temp = (unsigned long *)original_ptr;

    temp = (unsigned long *)realloc(temp, new_size_in_bytes);
    if (temp == NULL) {
        if (Memory_Error != NULL)
            Memory_Error();
        return NULL;
    }

    return (temp);
}

long Ram_Free(MemoryFlagType flag)
{
#if (0)
    GlobalCompact(0);
    MEMORYSTATUS mem_info;
    mem_info.dwLength = sizeof(mem_info);
    GlobalMemoryStatus(&mem_info);
    return (mem_info.dwAvailPhys);
#endif
    return (64 * 1024 * 1024);
}

long Heap_Size(MemoryFlagType flag)
{
    return Ram_Free(flag);
}

long Total_Ram_Free(MemoryFlagType flag)
{
    return Ram_Free(flag);
}
