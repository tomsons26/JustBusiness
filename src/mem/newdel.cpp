typedef enum
{
    MEM_NORMAL = 0x0000, // Default memory (normal).
    MEM_NEW = 0x0001, // Called by the operator new and was overloaded.
    MEM_CLEAR = 0x0002, // Clear memory before returning.
} MemoryFlagType;

void *__cdecl Alloc(unsigned long bytes_to_alloc, MemoryFlagType flags);
void __cdecl Free(void const *pointer);


void *operator new(size_t size)
{
    return (Alloc((unsigned long)size, MEM_NEW));
}

void *operator new[](size_t size)
{
    return (Alloc((unsigned long)size, MEM_NEW));
}

void operator delete(void *ptr)
{
    Free(ptr);
}
void operator delete[](void *ptr)
{
    Free(ptr);
}
