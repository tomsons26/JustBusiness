#pragma once
#include <cstdint>

struct _VQHeader
{
    uint32_t ImageSize;
    uint16_t ImageWidth;
    uint16_t ImageHeight;
    uint16_t BlockWidth;
    uint16_t BlockHeight;
    uint16_t BlockType;
    uint16_t PaletteRange;
    uint16_t Num1Color;
    uint16_t CodebookSize;
    uint16_t CodingFlag;
    uint16_t FrameDiffMethod;
    uint16_t ForcedPalette;
    uint16_t F555Palette;
    uint16_t VQVersion;
    uint16_t pad[5];
};
typedef _VQHeader VQHeader;

enum WallpaperFormat : int8_t
{
    WF_NO_WALLPAPER,
    WF_RAW_WALLPAPER,
    WF_BMP_WALLPAPER,
    WF_VQ_WALLPAPER,
};

class WallpaperClass
{
public:
    __cdecl WallpaperClass(char *name);
    __cdecl ~WallpaperClass();

    int32_t __cdecl Load_Wallpaper(char *name);
    WallpaperFormat __cdecl Load_VQ_Wallpaper(char *filename);
    void __cdecl WP_Convert_VQ_Vector_Format();

private:
    char base_filename[260];
    uint8_t *wallpaper_image_buffer;
    int32_t current_wallpaper_x;
    int32_t current_wallpaper_format;
    uint8_t *palette;
    VQHeader* vq_header;
    uint8_t *vq_codebook;
    uint16_t *vq_vectors;
    int32_t wallpaper_width_in_blocks;
    int32_t wallpaper_height_in_blocks;
    int32_t wallpaper_width_in_pixels;
    int32_t wallpaper_height_in_pixels;
    uint8_t *pixel_data;
};