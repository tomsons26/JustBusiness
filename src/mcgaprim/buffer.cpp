#include "buffer.h"
// temp
#include "mapview.h"

BufferClass::BufferClass(void *buffer, long size)
{
    Size = size;

    if (buffer) {
        Buffer = (unsigned char *)buffer;
        Allocated = FALSE;
    } else {
        Buffer = new unsigned char[Size];
        Allocated = TRUE;
    }
}
BufferClass::BufferClass(long size)
{
    Size = size;
    Buffer = new unsigned char[Size];
    Allocated = TRUE;
}
BufferClass::BufferClass()
{
    Buffer = NULL;
    Size = 0;
    Allocated = FALSE;
}

BufferClass::~BufferClass()
{
    if (Allocated) {
        delete[] Buffer;
    }
}
