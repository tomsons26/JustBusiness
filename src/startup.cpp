
#include "function.h"
#include "globals.h"
#include "mcgaprim/gbuffer.h"
#include "mem/wwmem.h"
#include "players.h"

#include <cstdio>

int main(int argc, char **argv)
{
    return 0;
}

#if 0

void Get_Executable_And_CD_Paths()
{
    char *i;

    if (__argc >= 1 && strlen(*(const char **)__argv) < 200) {
        strcpy(Text_String, *(const char **)__argv);
        for (i = &Text_String[strlen(Text_String) - 1]; *i != '\\'; --i) {
            ;
        }
        i[1] = 0;
        strcpy(Executable_Path, Text_String);
    }

    int exe_path_found = 'A';
    do {
        sprintf(Text_String, "%c:\\", exe_path_found);
        if (GetDriveType(Text_String) == DRIVE_CDROM) {
            strcpy(CD_Root_Path, Text_String);
            strcat(CD_Root_Path, "MONOPOLY\\");
            exe_path_found = 'Z';
        }
        ++exe_path_found;
    } while (exe_path_found <= 'Z');
}
#endif

int Get_Users_System_Configuration()
{
    HDC screen_dc = GetDC(NULL);
    int raster_caps = GetDeviceCaps(screen_dc, RASTERCAPS);
    Is_Palette_Device = raster_caps & RC_PALETTE;
    Is_Supported_Depth = GetDeviceCaps(screen_dc, BITSPIXEL) >= 8;

    System_Screen_Width = GetSystemMetrics(SM_CXSCREEN);
    System_Screen_Height = GetSystemMetrics(SM_CYSCREEN);

    System_Border_Width = GetSystemMetrics(SM_CXBORDER);
    System_Border_Height = GetSystemMetrics(SM_CYBORDER);

    System_Caption_Height = GetSystemMetrics(SM_CYCAPTION);

    System_Menu_Bar_Height = GetSystemMetrics(SM_CYMENU);

    ReleaseDC(NULL, screen_dc);
    return 1;
}

#if 0
void Allocate_Site_Receive_Strings()
{
    //int i = 0;
    //do {
    //    Site_Receive_Message_Strings[i] = (char *)Alloc(265, MEM_NORMAL);
    //    if (!Site_Receive_Message_Strings[i]) {
    //        Real_Error_Window_Message(982);
    //    }
    //    ++i;
    //} while (i < TOTAL_PLAYERS);
}

void Free_Site_Receive_Strings()
{
    //int i = 0;
    //do {
    //    if (Site_Receive_Message_Strings[i]) {
    //        Free(Site_Receive_Message_Strings[i]);
    //    }
    //    ++i;
    //} while (i < TOTAL_PLAYERS);
}
#endif

extern void Clear_System_Palette();

HPALETTE Create_Identity_Palette(RGBQUAD *rgb_ptr)
{
    int i;
    struct
    {
        WORD Version;
        WORD NumberOfEntries;
        PALETTEENTRY aEntries[256];
    } Palette = {768, 256};

    Clear_System_Palette();
    HDC device_context_handle = GetDC(NULL);

    if (Is_Palette_Device) {
        if (GetSystemPaletteUse(device_context_handle) == SYSPAL_NOSTATIC) {
            for (i = 1; i < 255; i++) {
                Palette.aEntries[i].peRed = rgb_ptr[i].rgbRed;
                Palette.aEntries[i].peGreen = rgb_ptr[i].rgbGreen;
                Palette.aEntries[i].peBlue = rgb_ptr[i].rgbBlue;
                Palette.aEntries[i].peFlags = PC_RESERVED;
            }

            Palette.aEntries[0].peRed = 0;
            Palette.aEntries[0].peGreen = 0;
            Palette.aEntries[0].peBlue = 0;
            Palette.aEntries[0].peFlags = 0;

            Palette.aEntries[255].peRed = 255;
            Palette.aEntries[255].peGreen = 255;
            Palette.aEntries[255].peBlue = 255;
            Palette.aEntries[255].peFlags = 0;
        } else {
            GetSystemPaletteEntries(device_context_handle, 0, 256, Palette.aEntries);

            for (i = 0; i < 10; i++) {
                Palette.aEntries[i].peFlags = 0;
            }

            for (i = 246; i < 256; i++) {
                Palette.aEntries[i].peFlags = PC_RESERVED;
            }

            for (i = 10; i < 246; i++) {
                Palette.aEntries[i].peRed = rgb_ptr[i].rgbRed;
                Palette.aEntries[i].peGreen = rgb_ptr[i].rgbGreen;
                Palette.aEntries[i].peBlue = rgb_ptr[i].rgbBlue;
                Palette.aEntries[i].peFlags = PC_RESERVED;
            }
        }
    }
    HPALETTE palette_handle = CreatePalette((LOGPALETTE *)&Palette);

    ReleaseDC(Main_Window_Handle, device_context_handle);

    return palette_handle;
}

void Clear_System_Palette()
{
    struct
    {
        WORD Version;
        WORD NumberOfEntries;
        PALETTEENTRY aEntries[256];
    } static Palette = {768, 256};

    for (int i = 0; i < 256; i++) {
        Palette.aEntries[i].peRed = 0;
        Palette.aEntries[i].peGreen = 0;
        Palette.aEntries[i].peBlue = 0;
        Palette.aEntries[i].peFlags = PC_RESERVED;
    }

    HDC device_context_handle = GetDC(Main_Window_Handle);
    HPALETTE palette_handle = CreatePalette((LOGPALETTE *)&Palette);

    if (palette_handle) {
        palette_handle = SelectPalette(device_context_handle, palette_handle, FALSE);
        RealizePalette(device_context_handle);
        palette_handle = SelectPalette(device_context_handle, palette_handle, FALSE);
        DeleteObject(palette_handle);
    }

    ReleaseDC(Main_Window_Handle, device_context_handle);
}