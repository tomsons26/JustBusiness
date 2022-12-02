#include "vq.h"
#include "file/file.h"
#include "globals.h"
#include "mem/wwmem.h"
#include <cstdint>
#include <cstdio>

extern void Update_Variable_Portion_Of_Palette(uint8_t *palette, int32_t first_palette_index, int32_t last_palette_index);

char *VQA_Mem_Pool = NULL;

VQA_Kind Type_Of_VQA_Currently_Playing = VK_NONE;

int16_t Frame_Synch_Data[52] = {
    675,
    0,
    15,
    30,
    45,
    60,
    75,
    90,
    105,
    120,
    135,
    180,
    195,
    210,
    225,
    240,
    255,
    270,
    285,
    300,
    315,
    360,
    375,
    390,
    405,
    420,
    435,
    450,
    465,
    480,
    495,
    540,
    555,
    570,
    585,
    600,
    615,
    630,
    645,
    660,
    675,
    720,
    735,
    750,
    765,
    780,
    795,
    810,
    825,
    840,
    855,
    900,
};

int32_t FUNC_004342CC()
{
    DWORD t = timeGetTime();

    if (t <= Monopoly_Start_Time) {
        return -1 - Monopoly_Start_Time + t;
    }

    return t - Monopoly_Start_Time;
}

__cdecl VQAClass::VQAClass(char *filename, char *buffer, uint16_t media_src, VQA_Kind type, int32_t id)
{
    VQA_DefaultConfig(&vqa_config);

    vqa_config.Vmode = 0;
    vqa_config.ImageBuf = (uint8_t *)buffer;

    if (DIB_Bottom_Up) {
        vqa_config.DrawFlags = 48;
    } else {
        vqa_config.DrawFlags = 0;
    }
#if 0
    vqa_config.DrawFlags |= 1;
    vqa_config.DrawFlags |= 4;

    if (buffer != WinG_Buffer_Ptr) {
        vqa_config.ImageWidth = 316;
        vqa_config.ImageHeight = 316;
        vqa_config.X1 = 0;
        vqa_config.Y1 = 0;
        goto LABEL_15;
    }

    vqa_config.ImageWidth = 640;
    vqa_config.ImageHeight = 440;
    if ((unsigned __int8)(type - VK_INTRO_PARKER_LOGO) >= (unsigned __int8)VK_MOVEMENT_STOP) {
        if ((_BYTE)type == VK_INTRO_SPOTLIGHT) {
            vqa_config.X1 = 100;
            vqa_config.Y1 = 0;
            goto LABEL_15;
        }
        if ((_BYTE)type != VK_INTRO_BOARD) {
            if ((unsigned __int8)(type - VK_INTRO_MOVEMENT_START) < (unsigned __int8)VK_MOVEMENT_LOOP) {
                vqa_config.X1 = 162;
            } else {
                vqa_config.X1 = 262;
            }
            vqa_config.Y1 = 62;
            goto LABEL_15;
        }
    }
    vqa_config.X1 = 0;
    vqa_config.Y1 = 0;
LABEL_15:

    if ((unsigned __int8)(type - 1) < (unsigned __int8)VK_MOVEMENT_LOOP) {
        goto LABEL_39;
    }

    if ((_BYTE)type == VK_GENERIC_WITH_TALL_WALLPAPER) {
        if (id != 1 && id != 2) {
            vqa_config.DrawerCallback = VQ_Drawer_Callback;
        } else {
            vqa_config.DrawerCallback = VQ_Drawer_Present_Card_Callback;
        }
        goto LABEL_26;
    }

    if ((_BYTE)type == VK_GO_TO_JAIL) {
    LABEL_39:
        vqa_config.DrawerCallback = VQ_Drawer_Movement_Callback;
    } else if ((unsigned __int8)(type - VK_INTRO_MOVEMENT_START) < (unsigned __int8)VK_MOVEMENT_LOOP) {
        vqa_config.DrawerCallback = VQ_Drawer_Intro_Movement_Callback;
    } else {
        vqa_config.DrawerCallback = VQ_Drawer_Callback;
    }

LABEL_26:
    vqa_config.FrameRate = -1;
    vqa_config.DrawRate = -1;
    vqa_handle = VQA_Alloc();
    media_source = media_src;

    media_src_1 = media_source;
    v7 = media_src_1 < 1;
    v8 = media_src_1 - 1;

    if (!v7 && !v8) {
        vqa_config.NumFrameBufs = 1;
        vqa_config.NumCBBufs = 2;
        VQA_Init(vqa_handle, Memory_VQA_Stream_Handler);
    } else {
        if ((_BYTE)type == VK_OFBP) {
            vqa_config.NumFrameBufs = 1;
            vqa_config.NumCBBufs = 2;
        } else {
            vqa_config.NumFrameBufs = 6;
            vqa_config.NumCBBufs = 3;
        }
        VQA_Init(vqa_handle, Disk_VQA_Stream_Handler);
    }
#endif
    file_handle = 0;

    type_of_vqa = type;
    Type_Of_VQA_Currently_Playing = type;

    identifier = id;

    current_frame = 0;
    total_frames = 0;

    strcpy(base_filename, filename);
    sprintf(vqa_filename, "%s.vqa", filename);

    if (type_of_vqa == VK_MOVEMENT_LOOP || type_of_vqa == VK_INTRO_MOVEMENT_LOOP) {
        sprintf(data_filename, "%s.%s", filename, "FSD");
    }

    vqa_is_open = 0;
}

int32_t VQAClass::Cache_VQA(uint32_t bytes_to_cache)
{
    struct VQACacheHeader
    {
        uint32_t file_offset;
        uint32_t file_size;
    };

    static VQACacheHeader vqa_header;
    static VQACacheHeader *vqa_cache_hdr;
    static char *vqa_cache_buffer;

    uint32_t size = bytes_to_cache;
    if (!file_handle) {
        file_handle = Open_File(vqa_filename, 1);
        if (file_handle == -1) {
            return -4;
        }

        vqa_header.file_size = File_Size(file_handle);
        vqa_header.file_offset = 0;

        vqa_cache_hdr =
            (VQACacheHeader *)Mem_Alloc(VQA_Mem_Pool, vqa_header.file_size + sizeof(VQACacheHeader), (uint32_t)vqa_handle);

        if (!vqa_cache_hdr) {
            Close_File(file_handle);
            return -3;
        }

        Mem_Copy(&vqa_header, vqa_cache_hdr, sizeof(VQACacheHeader));
        vqa_cache_buffer = (char *)&vqa_cache_hdr[1];
    }

    if (!bytes_to_cache) {
        size = vqa_header.file_size - vqa_header.file_offset;
    } else if (bytes_to_cache + vqa_header.file_offset > vqa_header.file_size) {
        size = vqa_header.file_size - vqa_header.file_offset;
    }

    if (size != Read_File(file_handle, vqa_cache_buffer + vqa_header.file_offset, size)) {
        Close_File(file_handle);
        vqa_header.file_offset = 0;
        file_handle = 0;
        return -5;
    }

    vqa_header.file_offset += size;
    if (vqa_header.file_offset >= vqa_header.file_size) {
        Close_File(file_handle);
        vqa_header.file_offset = 0;
        file_handle = 0;
        return -2;
    }

    return -1;
}

int32_t VQAClass::Update_Palette()
{
    switch (type_of_vqa) {
        case VK_MOVEMENT_START:
        case VK_MOVEMENT_STOP:
        case VK_MOVEMENT_LOOP:
        case VK_GENERIC_WITH_TALL_WALLPAPER:
        case VK_GO_TO_JAIL:
        case VK_INTRO_MOVEMENT_START:
        case VK_INTRO_MOVEMENT_STOP:
        case VK_INTRO_MOVEMENT_LOOP:
            Update_Variable_Portion_Of_Palette(palette, 24, 141);
            break;
        case VK_OFBP:
            break;
        default:
            Update_Variable_Portion_Of_Palette(palette, 24, 245);
            break;
    }
    return 1;
}

int32_t VQAClass::Open_And_Load_Buffers()
{
    _VQAInfo vqa_info;

    if (type_of_vqa == VK_MOVEMENT_LOOP || type_of_vqa == VK_INTRO_MOVEMENT_LOOP) {
        // Monopoly_SetCurrentDirectory(FSD_HD_PATH);
        int32_t fh = Open_File(data_filename, 1);
        if (fh != -1) {
            if (Read_File(fh, Frame_Synch_Data, sizeof(Frame_Synch_Data)) == sizeof(Frame_Synch_Data)) {
                // Wallpaper.Load_Shift_Data(fh);
            }
            Close_File(fh);
        }
    }

    // Monopoly_SetCurrentDirectory(MOVIES_CD_PATH);

    if (VQA_Open(vqa_handle, vqa_filename, &vqa_config)) {
        vqa_is_open = 0;
        return 0;
    }

    uint8_t *pal_ptr = (uint8_t *)VQA_GetPalette(vqa_handle);

    if (pal_ptr) {
        Mem_Copy(pal_ptr, palette, sizeof(palette));
        int i = 0;
        do {
            palette[i++] <<= 2;
        } while (i < sizeof(palette));
    }

    VQA_GetInfo(vqa_handle, &vqa_info);
    total_frames = vqa_info.NumFrames;
    vqa_is_open = 1;

    return 1;
}

// done
void VQAClass::Seek_To_Frame(uint32_t frame)
{
    if (frame != current_frame && VQA_SeekFrame(vqa_handle, frame, 0) != -1) {
        current_frame = frame;
    }
}

int32_t VQAClass::Play_VQA(int32_t frame_number)
{
    return 0;
}

int32_t VQAClass::Play_Intro_Movement_VQA(int32_t last_frame_to_play)
{
    return 0;
}

int32_t VQAClass::Play_Movement_VQA(int32_t last_frame_to_play)
{
    return 0;
}

int32_t VQAClass::Play_Generic_VQA(int32_t last_frame_to_play)
{
    return 0;
}

// done
void VQAClass::Play_VQA_Frame(int32_t frame_number)
{
    if (frame_number == -1) {
        frame_number = total_frames - 1;
    }

    if (frame_number != current_frame) {
        Seek_To_Frame(frame_number);
        current_frame = frame_number;
    }

    Play_VQA(frame_number);

    if (current_frame >= total_frames) {
        current_frame = 0;
    }
}

// done
void VQAClass::Pause_VQA()
{
    VQA_Play(vqa_handle, 3);
}

// done
void VQAClass::Close_And_Free_VQA()
{
    if (vqa_is_open) {
        VQA_Close(vqa_handle);
    }

    VQA_Free(vqa_handle);

    if (media_source == 1) {
        char *vqa_cache_ptr = (char *)Mem_Find(VQA_Mem_Pool, (uint32_t)vqa_handle);
        if (vqa_cache_ptr) {
            Mem_Free(VQA_Mem_Pool, vqa_cache_ptr);
        }
    }
}

// done
VQA_Kind VQAClass::Get_Type_Of_VQA()
{
    return type_of_vqa;
}

// done
int32_t VQAClass::Get_Id_Of_VQA()
{
    return identifier;
}

// done
int32_t VQAClass::Loop_Frame_For_Location(int32_t location)
{
    if (location < 0) {
        location = 0;
    }
    if (location >= 52) {
        location = 51;
    }
    return Frame_Synch_Data[location];
}

// done
void VQAClass::Set_Draw_Buffer(
    uint8_t *buffer, uint32_t buffer_width, uint32_t buffer_height, uint32_t x_offset, uint32_t y_offset)
{
    VQA_Set_DrawBuffer(vqa_handle, buffer, buffer_width, buffer_height, x_offset, y_offset);
    vqa_config.ImageBuf = buffer;
    vqa_config.ImageWidth = buffer_width;
    vqa_config.ImageHeight = buffer_height;
    vqa_config.X1 = x_offset;
    vqa_config.Y1 = y_offset;
}
extern int32_t Load_PMT_Data_File(char *str);

// done
int32_t VQAClass::Load_PMT_Data()
{
    return Load_PMT_Data_File(base_filename);
}
