// cant...................................................................................
//#include "shape.h"

#include <cstddef>

typedef struct
{
    unsigned short ShapeType;
    unsigned char Height;
    unsigned short Width;
    unsigned char OriginalHeight;
    unsigned short ShapeSize;
    unsigned short DataLength;
    unsigned char Colortable[16];
} Shape_Type;

typedef struct
{
    unsigned short NumShapes;
    long Offsets[];
} ShapeBlock_Type;

int __cdecl Get_Shape_Size(void const *shape)
{
    Shape_Type *shp = (Shape_Type *)shape;

    if (!shape)
        return (0);

    return (shp->ShapeSize);
}

int Get_Shape_Uncomp_Size(void const *shape)
{
    Shape_Type *shp = (Shape_Type *)shape;

    return (shp->DataLength);
}

short __cdecl Get_Shape_Data(void const *shape, short data)
{
    short *word_ptr = (short *)shape;
    short retval;

    retval = *(word_ptr - (data + 1));

    return (retval);
}

int __cdecl Extract_Shape_Count(void const *buffer)
{
    ShapeBlock_Type *block = (ShapeBlock_Type *)buffer;

    return (block->NumShapes);
}

void *__cdecl Extract_Shape(void const *buffer, int shape)
{
    ShapeBlock_Type *block = (ShapeBlock_Type *)buffer;
    long offset;
    char *bytebuf = (char *)buffer;

    if (!buffer || shape < 0 || shape >= block->NumShapes) {
        return (NULL);
    }

    offset = block->Offsets[shape];

    return (bytebuf + 2 + offset);
}

int Get_Shape_Width(void const *shape)
{
    Shape_Type *shp = (Shape_Type *)shape;

    return (shp->Width);
}

int Get_Shape_Height(void const *shape)
{
    Shape_Type *shp = (Shape_Type *)shape;

    return (shp->Height);
}

int __cdecl Set_Shape_Height(void const *shape, short newheight)
{
    Shape_Type *shp = (Shape_Type *)shape;
    short oldheight;

    oldheight = shp->Height;
    shp->Height = newheight;

    return (oldheight);
}

int __cdecl Restore_Shape_Height(void *shape)
{
    Shape_Type *shp = (Shape_Type *)shape;
    short oldheight;

    oldheight = shp->Height;
    shp->Height = shp->OriginalHeight;

    return (oldheight);
}

int Get_Shape_Original_Height(void const *shape)
{
    Shape_Type *shp = (Shape_Type *)shape;

    return (shp->OriginalHeight);
}