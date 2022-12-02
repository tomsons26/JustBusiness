
#include "globals.h"
#include "mcgaprim/gbuffer.h"

void Free_Palette()
{
    DeleteObject(Main_Palette_Handle);
    Main_Palette_Handle = NULL;
}
