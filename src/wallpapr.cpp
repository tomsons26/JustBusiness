#include "wallpapr.h"

#include "file/file.h"
#include "mem/wwmem.h"

#include <cstdio>
#include <cstring>
#include <intrin.h>

__cdecl WallpaperClass::WallpaperClass(char* name)
{
    wallpaper_image_buffer = 0;
    current_wallpaper_x = 0;
    palette = 0;
    current_wallpaper_format = 0;
    if (name) {
        //Monopoly_SetCurrentDirectory(BACKGROUNDS_CD_PATH);
        Load_Wallpaper(name);
    }
    //wat
    int v2 = __readfsdword(4u);
    ++*(int *)(v2 - 4);
}

__cdecl WallpaperClass::~WallpaperClass()
{
    int v2 = __readfsdword(4u);
    --*(int *)(v2 - 4);

    if (this) {
        if (wallpaper_image_buffer) {
            Free(wallpaper_image_buffer);
        }
    }
}

int32_t WallpaperClass::Load_Wallpaper(char *name)
{
    char buffer[260];

    sprintf(buffer, "%s.%s", name, "VQB");

    current_wallpaper_format = Load_VQ_Wallpaper(buffer);
    if (current_wallpaper_format == 3) {
        strcpy(base_filename, name);
        return 1;
    }

    return 0;
}


WallpaperFormat __cdecl WallpaperClass::Load_VQ_Wallpaper(char *filename)
{
    int32_t fh = Open_File(filename, 1);
    if (fh == -1) {
        return WF_NO_WALLPAPER;
    }

    int32_t file_size = File_Size(fh);
    if (file_size <= 0) {
        Close_File(fh);
        return WF_NO_WALLPAPER;
    }

    if (wallpaper_image_buffer) {
        Free(wallpaper_image_buffer);
    }

    wallpaper_image_buffer = (uint8_t*)Alloc(file_size, MEM_NORMAL);
    if (!wallpaper_image_buffer) {
        Close_File(fh);
        return WF_NO_WALLPAPER;
    }

    if (file_size != Read_File(fh, wallpaper_image_buffer, file_size)) {
        Close_File(fh);
        return WF_NO_WALLPAPER;
    }

    Close_File(fh);
    if (file_size < sizeof(VQHeader)) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    VQHeader *header = (VQHeader *)wallpaper_image_buffer;
    if (header->ImageSize != file_size) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    if (16 * (unsigned int)header->CodebookSize + 808 > file_size) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    if (header->ImageWidth != 640) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    if (header->ImageHeight != 224) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    if (header->BlockWidth != 4) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    if (header->BlockHeight != 4) {
        Free(wallpaper_image_buffer);
        return WF_NO_WALLPAPER;
    }

    WP_Convert_VQ_Vector_Format();

    return WF_VQ_WALLPAPER;
}

//temp
int __cdecl Convert_VQ_Vector_Format(unsigned __int8 *source, _VQHeader **header, unsigned __int8 **codebook,
    unsigned __int16 **vectors, unsigned __int8 **palette, int *width_in_pixels, int *height_in_pixels, int *width_in_blocks,
    int *height_in_blocks)
{
    return 0;
}

void __cdecl WallpaperClass::WP_Convert_VQ_Vector_Format()
{
    Convert_VQ_Vector_Format(wallpaper_image_buffer,
        &vq_header,
        &vq_codebook,
        &vq_vectors,
        &palette,
        &wallpaper_width_in_pixels,
        &wallpaper_height_in_pixels,
        &wallpaper_width_in_blocks,
        &wallpaper_height_in_blocks);
}