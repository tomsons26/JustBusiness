#pragma once

int Mem_Init(void *buffer, long size);
void *Mem_Alloc(void *poolptr, long lsize, unsigned long id);
int Mem_Free(void *poolptr, void *buffer);
void Mem_Reference(void *node);
void Mem_Lock_Block(void *node);
void Mem_In_Use(void *node);
void *Mem_Find(void *poolptr, unsigned long id);
unsigned long Mem_Get_ID(void *node);
void *Mem_Find_Oldest(void *poolptr);
void *Mem_Free_Oldest(void *poolptr);
long Mem_Pool_Size(void *poolptr);
long Mem_Avail(void *poolptr);
long Mem_Largest_Avail(void *poolptr);
void Mem_Cleanup(void *poolptr);

void __cdecl Mem_Copy(void const *source, void *dest, unsigned long bytes_to_copy);
void *Add_Long_To_Pointer(void const *ptr, long size);

typedef enum
{
    MEM_NORMAL = 0x0000,
    MEM_NEW = 0x0001,
    MEM_CLEAR = 0x0002,
    MEM_REAL = 0x0004,
    MEM_TEMP = 0x0008,
    MEM_LOCK = 0x0010,
} MemoryFlagType;

void *__cdecl Alloc(unsigned long bytes_to_alloc, MemoryFlagType flags);
void __cdecl Free(void const *pointer);
void *__cdecl Resize_Alloc(void const *original_ptr, unsigned long new_size_in_bytes);
long __cdecl Ram_Free(MemoryFlagType flag);
long __cdecl Total_Ram_Free(MemoryFlagType flag);
long __cdecl Heap_Size(MemoryFlagType flag);