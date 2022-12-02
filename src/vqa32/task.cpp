#include "vqaplay.h"
#include <cstdlib>
#include <cstring>

// done
VQAHandle *VQA_Alloc()
{
    VQAHandleP *vqa = (VQAHandleP *)malloc(sizeof(VQAHandleP));
    if (vqa) {
        memset(vqa, 0, sizeof(VQAHandleP));
    }
    return (VQAHandle *)vqa;
}

// done
void VQA_Free(VQAHandle *vqa)
{
    if (vqa) {
        free(vqa);
    }
}

// done
void VQA_Init(VQAHandle *vqa, stream_handler_func streamhandler)
{
    ((VQAHandleP *)vqa)->StreamHandler = streamhandler;
}

int32_t Check_Key();
void Get_Key();
void VQA_Configure_Drawer(VQAHandleP *vqap);

void VQA_SetTimer(int32_t int32, int32_t timer_method);
void VQA_InitMono(VQAHandleP *vqap);
void VQA_UpdateMono(VQAHandleP * vqap);
uint32_t VQA_GetTime();
int32_t VQA_LoadFrame(VQAHandle *vqa);
void User_Update(VQAHandle * vqa);

// done
int32_t VQA_Play(VQAHandle *vqa, int32_t mode)
{
    int32_t rc;
    int32_t key;

    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;

    if (!(vqabuf->Flags & 32)) {
        VQA_Configure_Drawer((VQAHandleP *)vqa);
        VQA_SetTimer(60 * drawer->CurFrame->FrameNum / config->DrawRate, config->TimerMethod);
        vqabuf->StartTime = VQA_GetTime();
        if (config->OptionFlags & 4) {
            VQA_InitMono((VQAHandleP *)vqa);
        }

        vqabuf->Flags |= 32;
    }

    if (mode >= 2 && mode == 3) {
        if (!(vqabuf->Flags & 64)) {
            vqabuf->Flags |= 64;
            vqabuf->EndTime = VQA_GetTime();
        }
        rc = -13;

    } else {
        if (vqabuf->Flags & 64) {
            vqabuf->Flags &= ~64;
            VQA_SetTimer(vqabuf->EndTime, config->TimerMethod);
        }
        while ((vqabuf->Flags & 24) != 24) {
            if (!(vqabuf->Flags & 16)) {
                rc = VQA_LoadFrame(vqa);
                if (!rc) {
                    ++vqabuf->LoadedFrames;
                } else if (rc != -8 && rc != -10) {
                    vqabuf->Flags |= 16;
                    rc = 0;
                }
            }
            if (!(config->DrawFlags & 2)) {
                rc = vqabuf->Draw_Frame(vqa);
                if (!rc) {
                    ++vqabuf->DrawnFrames;
                    User_Update(vqa);
                    rc = drawer->LastFrameNum;
                } else {
                    if (vqabuf->Flags & 16 && rc == -8) {
                        vqabuf->Flags |= 8;
                    }
                }
            } else {
                vqabuf->Flags |= 8;
                drawer->CurFrame->Flags = 0;
                drawer->CurFrame = drawer->CurFrame->Next;
            }
            if (config->OptionFlags & 4) {
                VQA_UpdateMono((VQAHandleP *)vqa);
            }
            if (mode == 1) {
                break;
            }
            if (config->OptionFlags & 2) {
                do {
                    key = Check_Key();
                } while (!key);
                Get_Key();
                if (key == 27) {
                    break;
                }
            }
            if (Check_Key()) {
                mode = 2;
                break;
            }
        }
    }

    if ((vqabuf->Flags & 24) == 24 || mode == 2) {
        vqabuf->EndTime = VQA_GetTime();
        rc = -1;
    }

    return rc;
}

//RA
int32_t VQA_SetStop(VQAHandle *vqa, int32_t frame)
{
    _VQAHeader *header = &((VQAHandleP *)vqa)->Header;
    int oldframes = -1;

    if (frame > 0 && header->Frames >= frame) {
        oldframes = header->Frames;
        header->Frames = frame;
    }

    return oldframes;
}

//Poly MAC
int32_t VQA_GetCurFrame(VQAHandle *vqa)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;

    return drawer->LastFrameNum;
}

// Poly MAC
_VQAConfig *VQA_GetConfig(VQAHandle *vqa)
{
    return &((VQAHandleP *)(vqa))->Config;
}

// done
int32_t VQA_SetDrawFlags(VQAHandle *vqa, int32_t flags)
{
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;

    int oldflags = config->DrawFlags;
    config->DrawFlags |= flags;

    return oldflags;
}

// done
int32_t VQA_ClearDrawFlags(VQAHandle *vqa, int32_t flags)
{
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;

    int oldflags = config->DrawFlags;
    config->DrawFlags &= ~flags;

    return oldflags;
}

// done
void VQA_GetInfo(VQAHandle *vqa, VQAInfo *info)
{
    VQAHeader *header = &((VQAHandleP *)vqa)->Header;

    info->NumFrames = header->Frames;
    info->ImageHeight = header->ImageHeight;
    info->ImageWidth = header->ImageWidth;
}

// done
void VQA_GetStats(VQAHandle *vqa, VQAStatistics *stats)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;

    stats->MemUsed = vqabuf->MemUsed;
    stats->StartTime = vqabuf->StartTime;
    stats->EndTime = vqabuf->EndTime;
    stats->FramesLoaded = vqabuf->LoadedFrames;
    stats->FramesDrawn = vqabuf->DrawnFrames;
    stats->FramesSkipped = vqabuf->Drawer.NumSkipped;
    stats->MaxFrameSize = vqabuf->Loader.MaxFrameSize;
    stats->SamplesPlayed = vqabuf->Audio.SamplesPlayed;
}

// done
char *VQA_Version()
{
    return "2.30";
}

// done
char *VQA_IDString()
{
    return "VQA32 2.30";
}

// done
void User_Update(VQAHandle *vqa)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;

    if (vqabuf->Flags & 1) {
        vqabuf->Page_Flip(vqa);
        vqabuf->Flipper.LastFrameNum = vqabuf->Flipper.CurFrame->FrameNum;
        vqabuf->Flipper.CurFrame->Flags = 0;
        vqabuf->Flags &= ~1;
    }
}