#pragma once
#include "vqa32/vqaplay.h"
#include <cstdint>

enum VQA_Kind : int8_t
{
    VK_NONE,

    VK_MOVEMENT_START,
    VK_MOVEMENT_STOP,
    VK_MOVEMENT_LOOP,

    VK_STATUS,

    VK_GENERIC,
    VK_GENERIC_WITH_TALL_WALLPAPER,

    VK_OFBP,

    VK_GO_TO_JAIL,

    VK_INTRO_PARKER_LOGO,
    VK_INTRO_WESTWOOD_LOGO,

    VK_INTRO_SPOTLIGHT,

    VK_INTRO_BOARD,

    VK_INTRO_MOVEMENT_START,
    VK_INTRO_MOVEMENT_STOP,
    VK_INTRO_MOVEMENT_LOOP,
};


class VQAClass
{
public:
    __cdecl VQAClass(char *filename, char *buffer, uint16_t media_src, VQA_Kind type, int32_t id);

    int32_t __cdecl Cache_VQA(uint32_t bytes_to_cache);

    int32_t __cdecl Update_Palette();

    int32_t __cdecl Open_And_Load_Buffers();

    void __cdecl Seek_To_Frame(uint32_t frame);

    int32_t __cdecl Play_VQA(int32_t frame_number);
    int32_t __cdecl Play_Intro_Movement_VQA(int32_t last_frame_to_play);
    int32_t __cdecl Play_Movement_VQA(int32_t last_frame_to_play);
    int32_t __cdecl Play_Generic_VQA(int32_t last_frame_to_play);

    void __cdecl Play_VQA_Frame(int32_t frame_number);
    void __cdecl Pause_VQA();
    void __cdecl Close_And_Free_VQA();

    VQA_Kind __cdecl Get_Type_Of_VQA();
    int32_t __cdecl Get_Id_Of_VQA();

    int32_t __cdecl Loop_Frame_For_Location(int32_t);

    void __cdecl Set_Draw_Buffer(
        uint8_t *buffer, uint32_t buffer_width, uint32_t buffer_height, uint32_t x_offset, uint32_t y_offset);

    int32_t __cdecl Load_PMT_Data();

private:
    char base_filename[260];
    char vqa_filename[260];
    char data_filename[260];
    VQAConfig vqa_config;
    VQAHandle *vqa_handle;
    uint16_t media_source;
    int32_t file_handle;
    uint8_t palette[768];
    VQA_Kind type_of_vqa;
    int32_t identifier;
    int32_t current_frame;
    int32_t total_frames;
    int32_t vqa_is_open;
};
