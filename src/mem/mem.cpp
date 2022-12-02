#include <cstring>

// temp
#include <windows.h>
template<class T>
T MAX(T a, T b)
{
    return (b > a) ? b : a;
};
#define TRUE 0x01
#define FALSE 0x00
#ifndef PRIVATE
#define PRIVATE static
#endif
#pragma comment(lib, "Winmm.lib")


//needs to be from global.h but won't let me include it
extern unsigned int Monopoly_Start_Time;

// i don't know what to do with this
inline int get_mem_time()
{
    //return (signed int)(unsigned __int64)((long double)(timeGetTime() - Monopoly_Start_Time) / 1000.0)
    //    >> 4;
    return 0;
}


#define MEM_BLOCK_IN_USE 0x00
#define MEM_BLOCK_LOCKED 0x01

typedef struct MemChain
{
    struct MemChain *Next;
    struct MemChain *Prev;
    unsigned long ID;
    unsigned short Time;
    unsigned long Size;
} MemChain_Type;

typedef struct MemPool
{
    MemChain_Type *FreeChain;
    MemChain_Type *UsedChain;
    unsigned long FreeMem;
    unsigned long TotalMem;
    long pad2;
} MemPool_Type;

PRIVATE void MemNode_Unlink(MemPool_Type *pool, int freechain, MemChain_Type *node);
PRIVATE void MemNode_Insert(
    MemPool_Type *pool, int freechain, MemChain_Type *node, unsigned int size, unsigned long id, int merge);

void __cdecl Mem_Copy(void const *source, void *dest, unsigned long bytes_to_copy);
void *Add_Long_To_Pointer(void const *ptr, long size);

int Mem_Init(void *buffer, long size)
{
    MemChain_Type *mem;
    MemPool_Type *pool;

    size = size & 0xFFFFFFF0L;

    if (!buffer || !size)
        return (FALSE);

    pool = (MemPool_Type *)buffer;
    pool->FreeMem = (size - sizeof(MemPool_Type)) >> 4;
    pool->UsedChain = NULL;
    pool->TotalMem = pool->FreeMem;
    mem = pool->FreeChain = (MemChain_Type *)(pool + 1);

    mem->Next = NULL;
    mem->Prev = NULL;
    mem->Size = pool->FreeMem;
    mem->ID = -1;
    mem->Time = 0;

    return (TRUE);
}

void *Mem_Alloc(void *poolptr, long lsize, unsigned long id)
{
    MemPool_Type *pool;
    MemChain_Type *node;
    unsigned int remainder = 0;
    int found;
    unsigned int size;

    if (!poolptr || !lsize)
        return (NULL);
    pool = (MemPool_Type *)poolptr;

    lsize += sizeof(MemChain_Type);
    lsize = (lsize + 0x0FL) & 0xFFFFFFF0L;
    size = lsize >> 4;

    if (pool->TotalMem < size) {
        return (NULL);
    }

    node = pool->FreeChain;
    found = FALSE;
    while (!found && node) {
        if (node->Size >= size) {
            found = TRUE;
            break;
        }
        node = node->Next;
    }
    if (!found) {
        return (NULL);
    }

    remainder = node->Size - size;

    if (remainder <= 2) {
        remainder = 0;
        size = node->Size;
    }

    MemNode_Unlink(pool, TRUE, node);

    if (remainder) {
        MemNode_Insert(pool, TRUE, (MemChain_Type *)Add_Long_To_Pointer(node, (long)size << 4), remainder, -1, FALSE);
    }

    MemNode_Insert(pool, FALSE, node, size, id, FALSE);

    pool->FreeMem -= size;

    return ((void *)(node + 1));
}

int Mem_Free(void *poolptr, void *buffer)
{
    MemPool_Type *pool;
    MemChain_Type *node;
    unsigned int size;

    if (!buffer || !poolptr) {
        return (FALSE);
    }
    pool = (MemPool_Type *)poolptr;

    node = (MemChain_Type *)buffer;
    node--;

    size = node->Size;
    MemNode_Unlink(pool, FALSE, node);
    MemNode_Insert(pool, TRUE, node, size, -1, TRUE);

    pool->FreeMem += size;

    return (TRUE);
}

void Mem_Reference(void *node)
{
    MemChain_Type *nodeptr;

    if (!node)
        return;

    nodeptr = (MemChain_Type *)node;
    nodeptr--;

    nodeptr->Time = get_mem_time() >> 4;
}

void Mem_Lock_Block(void *node)
{
    MemChain_Type *nodeptr;

    if (!node)
        return;

    nodeptr = (MemChain_Type *)node;
    nodeptr--;
    nodeptr->Time = MEM_BLOCK_LOCKED;
}

void Mem_In_Use(void *node)
{
    MemChain_Type *nodeptr;

    if (!node)
        return;

    nodeptr = (MemChain_Type *)node - 1;
    nodeptr->Time = MEM_BLOCK_IN_USE;
}

void *Mem_Find(void *poolptr, unsigned long id)
{
    MemPool_Type *pool;
    MemChain_Type *node;

    if (!poolptr)
        return (NULL);

    pool = (MemPool_Type *)poolptr;

    if (!pool->UsedChain) {
        return (NULL);
    }

    node = pool->UsedChain;
    while (node) {
        if (node->ID == id) {
            return (node + 1);
        }
        node = node->Next;
    }
    return (NULL);
}

unsigned long Mem_Get_ID(void *node)
{
    MemChain_Type *nodeptr;

    if (!node)
        return (0L);

    nodeptr = (MemChain_Type *)node - 1;
    return (nodeptr->ID);
}

void *Mem_Find_Oldest(void *poolptr)
{
    MemChain_Type *node;
    MemChain_Type *oldnode;
    unsigned int oldtime;
    unsigned int basetime;
    unsigned int time;

    if (!poolptr)
        return (NULL);

    oldnode = NULL;
    oldtime = 0;
    node = ((MemPool_Type *)poolptr)->UsedChain;

    //todo different timer
    basetime = (unsigned int)(get_mem_time() >> 4);

    while (node) {
        if (node->Time > MEM_BLOCK_LOCKED) {
            time = node->Time - basetime;

            if (time < oldtime || !oldnode) {
                oldtime = time;
                oldnode = node;
            }
        }
        node = node->Next;
    }

    if (oldnode) {
        oldnode++;
    }
    return (oldnode);
}

void *Mem_Free_Oldest(void *poolptr)
{
    MemChain_Type *node;

    if (!poolptr)
        return (NULL);
    node = (MemChain *)Mem_Find_Oldest(poolptr);
    if (Mem_Free(poolptr, node)) {
        return (node);
    }
    return (NULL);
}

long Mem_Pool_Size(void *poolptr)
{
    MemPool_Type *pool;
    long memtotal;

    if (!poolptr)
        return (NULL);

    pool = (MemPool_Type *)poolptr;

    memtotal = ((long)pool->TotalMem) << 4;
    memtotal -= sizeof(MemChain_Type);
    memtotal = MAX(memtotal, (long)0);
    return (memtotal);
}

long Mem_Avail(void *poolptr)
{
    MemPool_Type *pool;
    long memtotal;

    if (!poolptr)
        return (NULL);

    pool = (MemPool_Type *)poolptr;

    memtotal = ((long)pool->FreeMem) << 4;
    memtotal -= sizeof(MemChain_Type);
    memtotal = MAX(memtotal, (long)0);
    return (memtotal);
}

long Mem_Largest_Avail(void *poolptr)
{
    MemChain_Type *node;
    unsigned int size;
    long truesize;

    if (!poolptr)
        return (NULL);

    node = ((MemPool_Type *)poolptr)->FreeChain;
    size = 0;
    while (node) {
        if (node->Size >= size) {
            size = node->Size;
        }
        node = node->Next;
    }

    truesize = (long)size << 4;
    truesize -= sizeof(MemChain_Type);
    truesize = MAX(truesize, 0L);
    return (truesize);
}

void Mem_Cleanup(void *poolptr)
{
    MemPool_Type *pool;
    MemChain_Type *free, *cur;
    unsigned long size;
    unsigned long freesize;

    if (!poolptr)
        return;

    pool = (MemPool_Type *)poolptr;

    if (!pool->FreeMem || !pool->FreeChain || !pool->UsedChain)
        return;

    freesize = pool->FreeMem;
    free = pool->FreeChain;
    pool->FreeChain = NULL;
    cur = pool->UsedChain;
    while (TRUE) {
        while (cur < free && cur) {
            cur = cur->Next;
        }

        if (!cur) {
            break;
        }

        if (cur->Time == MEM_BLOCK_LOCKED) {
            size = (char *)cur - (char *)free;
            size >>= 4;
            freesize -= size;
            MemNode_Insert(pool, TRUE, free, (unsigned int)size, -1, FALSE);

            free = (MemChain_Type *)Add_Long_To_Pointer(cur, (unsigned long)cur->Size << 4);
            cur = cur->Next;
            while (free == cur) {
                free = (MemChain_Type *)Add_Long_To_Pointer(cur, (unsigned long)cur->Size << 4);
                cur = cur->Next;
            }

            if (!cur) {
                break;
            }
        } else {
            size = (unsigned long)cur->Size << 4;
            Mem_Copy(cur, free, size);
            cur = free;

            if (cur->Next) {
                cur->Next->Prev = cur;
            }
            if (cur->Prev) {
                cur->Prev->Next = cur;
            } else {
                pool->UsedChain = cur;
            }

            free = (MemChain_Type *)Add_Long_To_Pointer(cur, size);
        }
    }

    MemNode_Insert(pool, TRUE, free, freesize, -1, FALSE);
}

PRIVATE void MemNode_Unlink(MemPool_Type *pool, int freechain, MemChain_Type *node)
{
    MemChain_Type *other;
    MemChain_Type **chain;

    if (!pool || !node)
        return;

    if (freechain) {
        chain = &pool->FreeChain;
    } else {
        chain = &pool->UsedChain;
    }

    if (node->Prev) {
        other = node->Prev;
        other->Next = node->Next;
    } else {
        *chain = node->Next;
    }

    if (node->Next) {
        other = node->Next;
        other->Prev = node->Prev;
    }
}

PRIVATE void MemNode_Insert(
    MemPool_Type *pool, int freechain, MemChain_Type *node, unsigned int size, unsigned long id, int merge)
{
    MemChain_Type **chain;
    MemChain_Type *prev, *next;
    int doit = TRUE;

    if (!pool || !node || !size)
        return;

    if (freechain) {
        chain = &pool->FreeChain;
    } else {
        chain = &pool->UsedChain;
    }

    if (!*chain) {
        node->Next = NULL;
        node->Prev = NULL;
        node->Size = size;
        //todo different timer
        node->Time = (unsigned short)(get_mem_time() >> 4);
        node->ID = id;
        *chain = node;
        return;
    }

    prev = NULL;
    next = *chain;
    while (next && (next < node)) {
        prev = next;
        next = next->Next;
    }

    if (merge) {
        if (prev) {
            if (((char *)prev + ((long)prev->Size << 4)) == ((char *)node)) {
                prev->Size += size;
                size = prev->Size;
                node = prev;
                prev = prev->Prev;
                doit = FALSE;
            }
        }

        if (next) {
            if (((char *)node + ((long)size << 4)) == (char *)next) {
                if (!doit) {
                    node->Size += next->Size;
                    node->Next = next->Next;
                    next = next->Next;
                } else {
                    size += next->Size;
                    next = next->Next;
                }
            }
        }
    }

    if (prev) {
        prev->Next = node;
    } else {
        *chain = node;
    }

    if (next) {
        next->Prev = node;
    }

    if (doit) {
        node->Prev = prev;
        node->Next = next;
        node->Size = size;
        //node->Time = (unsigned short)(get_mem_time() >> 4);
        node->Time = 0;
        node->ID = id;
    }
}


void __cdecl Mem_Copy(void const *source, void *dest, unsigned long bytes_to_copy)
{
    memcpy(dest, source, bytes_to_copy);
}

void *Add_Long_To_Pointer(void const *ptr, long size)
{
    return ((void *)((char const *)ptr + size));
}
