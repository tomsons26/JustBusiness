#include "gbuffer.h"

// was in getmcga.asm but can't compile asm files.........
extern "C" {
UWORD Get_MCGA_Selector()
{
    __asm {
        push edx
        xor eax, eax
        mov ax, cs
        or ax, 7
        mov dx, 0x1C
        or ax, dx
        pop edx
        //retn
    }
}

UWORD Get_Standard_Selector()
{
    __asm {

        push edx
        xor eax, eax
        mov ax, ds
        pop edx
        //retn
    }
}
}

GraphicViewPortClass::GraphicViewPortClass(GraphicBufferClass *gbuffer, int x, int y, int w, int h)
{
    Attach(gbuffer, x, y, w, h);
}

GraphicViewPortClass::GraphicViewPortClass() {}

GraphicViewPortClass::~GraphicViewPortClass() {}

void GraphicViewPortClass::Attach(GraphicBufferClass *gbuffer, int x, int y, int w, int h)
{
    if (x < 0)
        x = 0;
    if (x >= gbuffer->Get_Width())
        x = gbuffer->Get_Width() - 1;
    if (y < 0)
        y = 0;
    if (y >= gbuffer->Get_Height())
        y = gbuffer->Get_Height() - 1;

    if (x + w > gbuffer->Get_Width())
        w = gbuffer->Get_Width() - x;

    if (y + h > gbuffer->Get_Height())
        h = gbuffer->Get_Height() - y;

    Selector = gbuffer->Get_Selector();

    Offset = gbuffer->Get_Offset() + (gbuffer->Get_Width() * y) + x;

    XPos = x;
    YPos = y;
    XAdd = gbuffer->Get_Width() - w;
    Width = w;
    Height = h;
    GraphicBuff = gbuffer;
}

int GraphicViewPortClass::Change(int x, int y, int w, int h)
{
    if (this == Get_Graphic_Buffer()) {
        return (FALSE);
    }

    Attach(Get_Graphic_Buffer(), x, y, w, h);
    return (TRUE);
}

GraphicBufferClass::GraphicBufferClass(long size, int w, int h, void *buffer)
{
    Size = size;

    if (buffer) {
        Buffer = (BYTE *)buffer;
        Allocated = FALSE;
    } else {
        Buffer = new BYTE[Size];
        Allocated = TRUE;
    }

    Selector = Get_Standard_Selector();

    Offset = (long)Buffer;

    Width = w;
    Height = h;

    XAdd = 0;
    XPos = 0;
    YPos = 0;
    GraphicBuff = this;
}

GraphicBufferClass::GraphicBufferClass(int w, int h, void *buffer)
{
    Size = w * h;

    if (buffer) {
        Buffer = (BYTE *)buffer;
        Allocated = FALSE;
    } else {
        if (!Size)
            Size = w * h;
        Buffer = new BYTE[Size];
        Allocated = TRUE;
    }

    Selector = Get_Standard_Selector();

    Offset = (long)Buffer;

    Width = w;
    Height = h;

    XAdd = 0;
    XPos = 0;
    YPos = 0;
    GraphicBuff = this;
}

GraphicBufferClass::~GraphicBufferClass() {}
