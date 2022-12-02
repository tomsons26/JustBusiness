#pragma once
#include <cstdint>

struct _VQAHandle
{
    uint32_t VQAStream;
};
typedef _VQAHandle VQAHandle;

struct _VQAHandle;

typedef uint32_t (*stream_handler_func)(VQAHandle *vqa, int32_t action, void *buffer, int32_t nbytes);
typedef void (*unvq_func)(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
typedef void (*drawer_callback_func)(uint8_t *buffer, int32_t frame);
typedef int32_t (*draw_frame_func)(VQAHandle *vqa);
typedef void (*page_flip_func)(VQAHandle *vqa);

struct _VQAFrameNode;
struct _VQACBNode;

struct _VQALoader
{
    _VQACBNode *CurCB;
    _VQACBNode *FullCB;
    _VQAFrameNode *CurFrame;
    int32_t NumPartialCB;
    int32_t PartialCBSize;
    int32_t CurFrameNum;
    int32_t LastCBFrame;
    int32_t LastFrameNum;
    int32_t WaitsOnDrawer;
    int32_t WaitsOnAudio;
    int32_t FrameSize;
    int32_t MaxFrameSize;
};
typedef _VQALoader VQALoader;

struct _VQAFlipper
{
    _VQAFrameNode *CurFrame;
    int32_t LastFrameNum;
};
typedef _VQAFlipper VQAFlipper;

struct _VQAConfig
{
    drawer_callback_func DrawerCallback;
    int32_t Vmode;
    int32_t VBIBit;
    uint8_t *ImageBuf;
    int32_t ImageWidth;
    int32_t ImageHeight;
    int32_t X1;
    int32_t Y1;
    int32_t FrameRate;
    int32_t DrawRate;
    int32_t TimerMethod;
    int32_t DrawFlags;
    int32_t OptionFlags;
    int32_t NumFrameBufs;
    int32_t NumCBBufs;
    char *VocFile;
    uint8_t *AudioBuf;
    int32_t AudioBufSize;
    int32_t AudioRate;
    int32_t Volume;
    int32_t HMIBufSize;
    int32_t DigiHandle;
    int32_t DigiCard;
    int32_t DigiPort;
    int32_t DigiIRQ;
    int32_t DigiDMA;
};
typedef _VQAConfig VQAConfig;

struct _VQADrawer
{
    _VQAFrameNode *CurFrame;
    uint32_t Flags;
    void *Display; // DisplayInfo*
    uint8_t *ImageBuf;
    int32_t ImageWidth;
    int32_t ImageHeight;
    int32_t X1;
    int32_t Y1;
    int32_t X2;
    int32_t Y2;
    int32_t ScreenOffset;
    int32_t CurPalSize;
    uint8_t Palette_24[768];
    uint8_t Palette_15[512];
    int32_t BlocksPerRow;
    int32_t NumRows;
    int32_t NumBlocks;
    int32_t MaskStart;
    int32_t MaskWidth;
    int32_t MaskHeight;
    int32_t LastTime;
    int32_t LastFrame;
    int32_t LastFrameNum;
    int32_t DesiredFrame;
    int32_t NumSkipped;
    int32_t WaitsOnFlipper;
    int32_t WaitsOnLoader;
};
typedef _VQADrawer VQADrawer;

struct _VQAFrameNode
{
    uint8_t *Pointers;
    _VQACBNode *Codebook;
    uint8_t *Palette;
    _VQAFrameNode *Next;
    uint32_t Flags;
    int32_t FrameNum;
    int32_t PtrOffset;
    int32_t PalOffset;
    int32_t PaletteSize;
};
typedef _VQAFrameNode VQAFrameNode;

struct _VQACBNode
{
    uint8_t *Buffer;
    _VQACBNode *Next;
    uint32_t Flags;
    uint32_t CBOffset;
};
typedef _VQACBNode VQACBNode;

struct _VQAStatistics
{
    int32_t StartTime;
    int32_t EndTime;
    int32_t FramesLoaded;
    int32_t FramesDrawn;
    int32_t FramesSkipped;
    int32_t MaxFrameSize;
    uint32_t SamplesPlayed;
    uint32_t MemUsed;
};
typedef _VQAStatistics VQAStatistics;

struct _VQAAudio
{
    uint8_t *Buffer;
    uint32_t AudBufPos;
    int16_t *IsLoaded;
    uint32_t NumAudBlocks;
    uint8_t *TempBuf;
    uint32_t TempBufSize;
    uint32_t TempBufLen;
    uint32_t Flags;
    uint32_t PlayPosition;
    uint32_t SamplesPlayed;
    uint32_t NumSkipped;
    uint16_t SampleRate;
    uint8_t Channels;
    uint8_t BitsPerSample;
    uint8_t sSOSInfo[0x28]; //_SOS_COMPRESS_INFO
};
typedef _VQAAudio VQAAudio;

struct _VQAData
{
    draw_frame_func Draw_Frame;
    page_flip_func Page_Flip;
    unvq_func UnVQ;
    _VQAFrameNode *FrameData;
    _VQACBNode *CBData;
    _VQAAudio Audio;
    _VQALoader Loader;
    _VQADrawer Drawer;
    _VQAFlipper Flipper;
    uint32_t Flags;
    int32_t *Foff;
    int32_t VBIBit;
    int32_t Max_CB_Size;
    int32_t Max_Pal_Size;
    int32_t Max_Ptr_Size;
    int32_t LoadedFrames;
    int32_t DrawnFrames;
    int32_t StartTime;
    int32_t EndTime;
    int32_t MemUsed;
};
typedef _VQAData VQAData;

struct _VQAHeader
{
    uint16_t Version;
    uint16_t Flags;
    uint16_t Frames;
    uint16_t ImageWidth;
    uint16_t ImageHeight;
    uint8_t BlockWidth;
    uint8_t BlockHeight;
    uint8_t FPS;
    uint8_t Groupsize;
    uint16_t Num1Colors;
    uint16_t CBentries;
    uint16_t Xpos;
    uint16_t Ypos;
    uint16_t MaxFramesize;
    uint16_t SampleRate;
    uint8_t Channels;
    uint8_t BitsPerSample;
    uint16_t Streams;
    uint16_t FutureUse[6];
};
typedef _VQAHeader VQAHeader;

struct _VQAInfo
{
    int32_t NumFrames;
    int32_t ImageWidth;
    int32_t ImageHeight;
};
typedef _VQAInfo VQAInfo;

struct _VQAHandleP
{
    uint32_t VQAStream;
    stream_handler_func StreamHandler;
    VQAData *VQABuf;
    VQAConfig Config;
    VQAHeader Header;
    int32_t vocfh;
};
typedef _VQAHandleP VQAHandleP;

VQAHandle *VQA_Alloc();
    void VQA_Free(VQAHandle *vqa);

int32_t VQA_Open(VQAHandle *vqa, char *filename, VQAConfig *config);
void VQA_Close(VQAHandle *vqa);

int32_t VQA_Play(VQAHandle *vqa, int32_t mode);
int32_t VQA_SeekFrame(VQAHandle *vqa, int32_t framenum, int32_t fromwhere);

void VQA_Set_DrawBuffer(VQAHandle *vqa, uint8_t *buffer, uint32_t width, uint32_t height, int32_t xpos, int32_t ypos);

char *VQA_GetPalette(VQAHandle *vqa);
int32_t VQA_GetPaletteSize(VQAHandle *vqa);

void VQA_DefaultConfig(VQAConfig *config);

void VQA_GetInfo(VQAHandle *vqa, VQAInfo *info);