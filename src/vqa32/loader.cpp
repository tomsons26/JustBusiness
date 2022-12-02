#include "vqaplay.h"

#include "../vqm32/iff.h"
#include <crtdbg.h>
#include <cstdlib>
#include <cstring>

void VQA_Close(VQAHandle *vqa);
void VQA_DefaultConfig(VQAConfig *config);
VQAData *AllocBuffers(VQAHeader *header, VQAConfig *config);
void FreeBuffers(VQAData *vqa, VQAConfig *config);
int32_t PrimeBuffers(VQAHandle *vqa);
int32_t TestVBIBit();
void VQA_Configure_Drawer(VQAHandleP *vqap);
int32_t VQA_LCW_Uncompress(uint8_t *str, uint8_t *palette, int32_t int32, int32_t int33);

extern int32_t Load_VQF(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_FINF(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_VQHD(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_CBF0(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_CBFZ(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_CBP0(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_CBPZ(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_CPL0(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_CPLZ(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_VPT0(VQAHandleP *vqa, uint32_t uint32);
extern int32_t Load_VPTZ(VQAHandleP *vqa, uint32_t uint32);

// done
int32_t VQA_Open(VQAHandle *vqa, char *filename, VQAConfig *config)
{
    ChunkHeader chunk;

    VQAHandleP *vqap = (VQAHandleP *)vqa;
    VQAHeader *header = &vqap->Header;

    if (vqap->StreamHandler(vqa, 3, filename, 0)) {
        return -2;
    }

    if (vqap->StreamHandler(vqa, 5, &chunk, 8)) {
        VQA_Close(vqa);
        return -3;
    }

    if (chunk.id != 'MROF' || !chunk.size) {
        VQA_Close(vqa);
        return -6;
    }

    if (vqap->StreamHandler(vqa, 5, &chunk, 4)) {
        VQA_Close(vqa);
        return -3;
    }

    if (chunk.id != 'AQVW') {
        VQA_Close(vqa);
        return -6;
    }

    if (vqap->StreamHandler(vqa, 5, &chunk, 8)) {
        VQA_Close(vqa);
        return -3;
    }

    chunk.size =
        (chunk.size << 24) & 0xFF000000 | (chunk.size << 8) & 0xFF0000 | (chunk.size >> 8) & 0xFF00 | (chunk.size >> 24);

    if (chunk.id != 'DHQV' || chunk.size != sizeof(VQAHeader)) {
        VQA_Close(vqa);
        return -6;
    }

    if (vqap->StreamHandler(vqa, 5, header, sizeof(VQAHeader))) {
        VQA_Close(vqa);
        return -3;
    }

    if (config) {
        memcpy(&vqap->Config, config, sizeof(VQAConfig));
    } else {
        VQA_DefaultConfig(&((VQAHandleP *)vqa)->Config);
    }

    if (vqap->Config.ImageWidth == -1) {
        vqap->Config.ImageWidth = header->ImageWidth;
    }

    if (vqap->Config.ImageHeight == -1) {
        vqap->Config.ImageHeight = header->ImageHeight;
    }

    vqap->VQABuf = AllocBuffers(header, &((VQAHandleP *)vqa)->Config);

    if (!vqap->VQABuf) {
        VQA_Close(vqa);
        return -7;
    }

    if (vqap->Config.FrameRate == -1) {
        vqap->Config.FrameRate = header->FPS;
    }
    if (vqap->Config.DrawRate == -1) {
        vqap->Config.DrawRate = header->FPS;
    }
    if (vqap->Config.DrawRate == -1 || !vqap->Config.DrawRate) {
        vqap->Config.DrawRate = header->FPS;
    }
    if (vqap->Config.VBIBit == -1) {
        vqap->Config.VBIBit = TestVBIBit();
    }

    vqap->VQABuf->VBIBit = vqap->Config.VBIBit;

    if (!(header->Flags & 1)) {
        vqap->Config.OptionFlags &= ~1;
    }

    if (PrimeBuffers(vqa)) {
        VQA_Close(vqa);
        return -3;
    }

    return 0;
}

// done
void VQA_Close(VQAHandle *vqa)
{
    if (((VQAHandleP *)vqa)->VQABuf) {
        FreeBuffers(((VQAHandleP *)vqa)->VQABuf, &((VQAHandleP *)vqa)->Config);
    }

    ((VQAHandleP *)vqa)->StreamHandler(vqa, 4, NULL, 0);
}

// DOS PLAYVQA
// VQFR
// VQFK
// FINF
// VQHD
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPT0
// VPTZ
// SND0
// default

// DOS MALCOM
// VQFR
// VQFK
// FINF
// VQHD
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPT0
// VPTZ
// SND0
// SND1
// default

// MAC MALCOM INSTALL
// VQFR
// VQFK
// FINF
// VQHD
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPT0
// VPTD
// VPTK
// SND0
// SND1
// SND2
// default

// BR
// SND0
// CBF0
// CPL0
// CBP0
// SNA1
// SND1
// SNA2
// SND2
// VPTD
// SN2J
// VQFK
// VPTK
// VQFL
// VQFR
// VPTR
// CBFZ
// VPKZ
// CPLZ
// CBPZ
// VPRZ
// VPTZ

// RA
// VQFR
// VQFK
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPT0
// VPTD
// VPTK
// SND0
// SNA0
// SND1
// SNA1
// SND2
// SNA2
// unknown pos VPTZ

// LOLPREVIEW
// VQFR
// VQFK
// VQFL
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPTK
// VPTR
// VPTD
// VPKZ
// VPRZ
// VPTZ
// SND0
// SNA0
// SND1
// SNA1
// SND2
// SNA2
// default

// POLY
// VQFR
// VQFK
// FINF
// VQHD
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPT0
// VPTR
// VPRZ
// VPTZ
// VPTK
// default

// MAC POLY
// VQFR
// VQFK
// FINF
// VQHD
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPT0
// VPTR
// VPRZ
// VPTD
// VPTK
// default

// todo
int32_t VQA_LoadFrame(VQAHandle *vqa)
{
    uint32_t iffsize = 0;
    _ChunkHeader chunk;

    int32_t frame_loaded = 0;
    VQAHandleP *vqap = (VQAHandleP *)vqa;
    VQAData *vqabuf = vqap->VQABuf;
    VQALoader *loader = &vqabuf->Loader;
    VQADrawer *drawer = &vqabuf->Drawer;
    VQAFrameNode *curframe = loader->CurFrame;

    if (curframe->Flags & 1) {
        ++vqabuf->Loader.WaitsOnDrawer;
        return -8;
    }

    if (!(vqabuf->Flags & 4)) {
        frame_loaded = 0;
        vqabuf->Loader.FrameSize = 0;
        curframe->Codebook = loader->FullCB;
    }

    while (!frame_loaded) {
        if (!(vqabuf->Flags & 4)) {
            if (vqap->StreamHandler(vqa, 5, &chunk, sizeof(chunk))) {
                return -1;
            }

            iffsize = (chunk.size << 24) & 0xFF000000 | (chunk.size << 8) & 0xFF0000 | (chunk.size >> 8) & 0xFF00
                | (chunk.size >> 24);
            loader->FrameSize += iffsize;
        }

#if 0
        if (chunk.id <= 'KTPV') {
            if (chunk.id == 'KTPV') {
                if (Load_VPTZ(vqap, iffsize)) {
                    return -3;
                }
                curframe->Flags |= 2u;
                frame_loaded = 1;
                continue;
            }
            if (chunk.id <= 'DHQV') {
                switch (chunk.id) {
                    case 'DHQV':
                        if (Load_VQHD(vqap, iffsize)) {
                            return -3;
                        }
                        continue;
                    case '0FBC':
                        if (Load_CBF0(vqap, iffsize)) {
                            return -3;
                        }
                        continue;
                    case '0LPC':
                        if (Load_CPL0(vqap, iffsize)) {
                            return -3;
                        }
                        if (!drawer->CurPalSize) {
                            memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
                            drawer->CurPalSize = curframe->PaletteSize;
                        }
                        curframe->Flags |= 4u;
                        continue;
                    case '0PBC':
                        if (Load_CBP0(vqap, iffsize)) {
                            return -3;
                        }
                        continue;
                    case '0TPV':
                        if (Load_VPT0(vqap, iffsize)) {
                            return -3;
                        }
                        frame_loaded = 1;
                        continue;
                }
            } else {
                switch (chunk.id) {
                    case 'DTPV':
                        goto LABEL_69;
                    case 'FNIF':
                        if (Load_FINF(vqap, iffsize)) {
                            return -3;
                        }
                        continue;
                    case 'KFQV':
                        if (Load_VQF(vqap, iffsize)) {
                            return -3;
                        }
                        curframe->Flags |= 2u;
                        frame_loaded = 1;
                        continue;
                }
            }
        } else if (chunk.id <= 'ZLPC') {
            switch (chunk.id) {
                case 'ZLPC':
                    if (Load_CPLZ(vqap, iffsize)) {
                        return -3;
                    }
                    if (!drawer->CurPalSize) {
                        drawer->CurPalSize = VQA_LCW_Uncompress(&curframe->Palette[curframe->PalOffset],
                            drawer->Palette_24,
                            vqabuf->Max_Pal_Size,
                            vqabuf->Max_Pal_Size - curframe->PalOffset);
                    }
                    curframe->Flags |= 4u;
                    continue;
                case 'RFQV':
                    if (Load_VQF(vqap, iffsize)) {
                        return -3;
                    }
                    frame_loaded = 1;
                    continue;
                case 'RTPV':
                    if (Load_VPT0(vqap, iffsize)) {
                        return -3;
                    }
                    frame_loaded = 1;
                    continue;
                case 'ZFBC':
                    if (Load_CBFZ(vqap, iffsize)) {
                        return -3;
                    }
                    continue;
            }
        } else {
            switch (chunk.id) {
                case 'ZPBC':
                    if (Load_CBPZ(vqap, iffsize)) {
                        return -3;
                    }
                    continue;
                case 'ZRPV':
                    if (Load_VPTZ(vqap, iffsize)) {
                        return -3;
                    }
                    frame_loaded = 1;
                    continue;
                case 'ZTPV':
                LABEL_69:
                    if (Load_VPTZ(vqap, iffsize)) {
                        return -3;
                    }
                    frame_loaded = 1;
                    continue;
            }
        }
#endif
        switch (chunk.id) {
            case 'RFQV':
                if (Load_VQF(vqap, iffsize)) {
                    return -3;
                }
                frame_loaded = 1;
                break;
            case 'KFQV':
                if (Load_VQF(vqap, iffsize)) {
                    return -3;
                }
                curframe->Flags |= 2;
                frame_loaded = 1;
                break;
            case 'FNIF':
                if (Load_FINF(vqap, iffsize)) {
                    return -3;
                }
                break;
            case 'DHQV':
                if (Load_VQHD(vqap, iffsize)) {
                    return -3;
                }
                break;
            case '0FBC':
                if (Load_CBF0(vqap, iffsize)) {
                    return -3;
                }
                break;
            case 'ZFBC':
                if (Load_CBFZ(vqap, iffsize)) {
                    return -3;
                }
                break;
            case '0PBC':
                if (Load_CBP0(vqap, iffsize)) {
                    return -3;
                }
                continue;
            case 'ZPBC':
                if (Load_CBPZ(vqap, iffsize)) {
                    return -3;
                }
                break;
            case '0LPC':
                if (Load_CPL0(vqap, iffsize)) {
                    return -3;
                }
                if (!drawer->CurPalSize) {
                    memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
                    drawer->CurPalSize = curframe->PaletteSize;
                }
                curframe->Flags |= 4;
                break;
            case 'ZLPC':
                if (Load_CPLZ(vqap, iffsize)) {
                    return -3;
                }
                if (!drawer->CurPalSize) {
                    drawer->CurPalSize = VQA_LCW_Uncompress(&curframe->Palette[curframe->PalOffset],
                        drawer->Palette_24,
                        vqabuf->Max_Pal_Size,
                        vqabuf->Max_Pal_Size - curframe->PalOffset);
                }
                curframe->Flags |= 4;
                break;
            case '0TPV':
                if (Load_VPT0(vqap, iffsize)) {
                    return -3;
                }
                frame_loaded = 1;
                break;
            ///??
            case 'RTPV':
                if (Load_VPT0(vqap, iffsize)) {
                    return -3;
                }
                frame_loaded = 1;
                break;
            ///??
            case 'ZRPV':
                if (Load_VPTZ(vqap, iffsize)) {
                    return -3;
                }
                frame_loaded = 1;
                break;
            ///??
            case 'ZTPV':
                if (Load_VPTZ(vqap, iffsize)) {
                    return -3;
                }
                frame_loaded = 1;
                break;
            ///??
            case 'KTPV':
                if (Load_VPTZ(vqap, iffsize)) {
                    return -3;
                }
                curframe->Flags |= 2;
                frame_loaded = 1;
                break;
            ///??
            case 'DTPV':
                if (Load_VPTZ(vqap, iffsize)) {
                    return -3;
                }
                frame_loaded = 1;
                break;
                // case SND0:
                // case SNA0:
                // case SND1:
                // case SNA1:
                // case SND2:
                // case SNA2:

            default:
                if (vqap->StreamHandler(vqa, 7, (void *)1, (iffsize + 1) & 0xFFFFFFFE)) {
                    return -5;
                }
                break;
        }
    };

    if (loader->CurFrameNum > 0 && loader->FrameSize > loader->MaxFrameSize) {
        loader->MaxFrameSize = loader->FrameSize;
    }

    curframe->FrameNum = loader->CurFrameNum++;
    loader->LastFrameNum = loader->CurFrameNum;
    curframe->Flags |= 1;
    loader->CurFrame = curframe->Next;

    return 0;
}

// done
int32_t VQA_SeekFrame(VQAHandle *vqa, int32_t framenum, int32_t fromwhere)
{
    int32_t rc = -1;

    VQAHandleP *vqap = (VQAHandleP *)vqa;
    VQAData *vqabuf = vqap->VQABuf;
    VQALoader *loader = &vqabuf->Loader;
    VQAHeader *header = &vqap->Header;

    if (header->Frames <= framenum || !vqabuf->Foff) {
        return rc;
    }

    int32_t group = framenum / header->Groupsize * header->Groupsize;
    if (group >= header->Groupsize) {
        group -= header->Groupsize;
    }

    if (vqap->StreamHandler(vqa, 7, NULL, 2 * (vqabuf->Foff[group] & 0xFFFFFFF))) {
        return rc;
    }

    loader->NumPartialCB = 0;
    loader->PartialCBSize = 0;
    loader->FullCB = vqabuf->CBData;
    loader->CurCB = vqabuf->CBData;
    loader->CurFrameNum = group;

    for (int i = 0; framenum - group > i; ++i) {
        loader->CurFrame->Flags = 0;
        rc = VQA_LoadFrame(vqa);
        if (rc) {
            if (rc != -8 && rc != -10) {
                rc = -1;
                break;
            }
            rc = 0;
        }
    }

    if (rc) {
        return rc;
    }

    loader->CurFrame->Flags = 0;

    // todo bit ugly, probably was a while loop
    for (VQAFrameNode *frame = loader->CurFrame->Next; frame != loader->CurFrame; frame = frame->Next) {
        frame->Flags = 0;
    }

    vqabuf->Drawer.CurFrame = loader->CurFrame;

    if (!PrimeBuffers(vqa)) {
        return framenum;
    }

    return -1;
}

VQAData *AllocBuffers(VQAHeader *header, VQAConfig *config)
{
    VQACBNode *this_cb;
    VQACBNode *cbnode;
    VQAFrameNode *this_frame;
    VQAFrameNode *framenode;

    if (!config->NumCBBufs || !config->NumFrameBufs || config->AudioBufSize < config->HMIBufSize) {
        return NULL;
    }

    VQAData *vqabuf = (VQAData *)malloc(sizeof(VQAData));

    if (!vqabuf) {
        return NULL;
    }

    memset(vqabuf, 0, sizeof(VQAData));
    vqabuf->MemUsed = sizeof(VQAData);

    vqabuf->Drawer.LastTime = -60;
    vqabuf->Max_CB_Size = (header->BlockHeight * header->BlockWidth * header->CBentries + 0xFA) & 0xFFFC;
    vqabuf->Max_Pal_Size = 1792;
    vqabuf->Max_Ptr_Size = (2 * (unsigned short)(header->ImageHeight / (int)header->BlockHeight)
                                   * (unsigned short)(header->ImageWidth / (int)header->BlockWidth)
                               + 1024)
        & 0xFFFC;
    vqabuf->Loader.LastCBFrame = header->Groupsize * ((header->Frames - 1) / header->Groupsize);

    for (int i = 0; config->NumCBBufs > i; ++i) {
        cbnode = (VQACBNode *)malloc(vqabuf->Max_CB_Size + sizeof(VQACBNode));
        if (!cbnode) {
            FreeBuffers(vqabuf, config);
            return NULL;
        }

        vqabuf->MemUsed += vqabuf->Max_CB_Size + sizeof(VQACBNode);
        memset(cbnode, 0, sizeof(VQACBNode));
        cbnode->Buffer = (uint8_t *)&cbnode[1];
        if (!i) {
            vqabuf->CBData = cbnode;
        } else {
            this_cb->Next = cbnode;
        }
        this_cb = cbnode;
    }

    cbnode->Next = vqabuf->CBData;
    vqabuf->Loader.CurCB = vqabuf->CBData;
    vqabuf->Loader.FullCB = vqabuf->CBData;

    for (int i = 0; config->NumFrameBufs > i; ++i) {
        framenode = (VQAFrameNode *)malloc(vqabuf->Max_Pal_Size + vqabuf->Max_Ptr_Size + sizeof(VQAFrameNode));
        if (!framenode) {
            FreeBuffers(vqabuf, config);
            return NULL;
        }
        vqabuf->MemUsed += vqabuf->Max_Pal_Size + vqabuf->Max_Ptr_Size + sizeof(VQAFrameNode);
        memset(framenode, 0, sizeof(VQAFrameNode));
        framenode->Pointers = (uint8_t *)&framenode[1];
        framenode->Palette = (uint8_t *)&framenode[1] + vqabuf->Max_Ptr_Size;
        framenode->Codebook = vqabuf->CBData;
        if (!i) {
            vqabuf->FrameData = framenode;
        } else {
            this_frame->Next = framenode;
        }
        this_frame = framenode;
    }

    framenode->Next = vqabuf->FrameData;
    vqabuf->Loader.CurFrame = vqabuf->FrameData;
    vqabuf->Drawer.CurFrame = vqabuf->FrameData;
    vqabuf->Flipper.CurFrame = vqabuf->FrameData;

    if (config->ImageBuf) {
        vqabuf->Drawer.ImageBuf = config->ImageBuf;
        vqabuf->Drawer.ImageWidth = config->ImageWidth;
        vqabuf->Drawer.ImageHeight = config->ImageHeight;
    } else if (config->DrawFlags & 1) {
        vqabuf->Drawer.ImageBuf = (uint8_t *)malloc(header->ImageHeight * header->ImageWidth);
        if (!vqabuf->Drawer.ImageBuf) {
            FreeBuffers(vqabuf, config);
            return NULL;
        }

        vqabuf->Drawer.ImageWidth = header->ImageWidth;
        vqabuf->Drawer.ImageHeight = header->ImageHeight;
        vqabuf->MemUsed += header->ImageHeight * header->ImageWidth;
    } else {
        vqabuf->Drawer.ImageWidth = config->ImageWidth;
        vqabuf->Drawer.ImageHeight = config->ImageHeight;
    }

    vqabuf->Foff = (int32_t *)malloc(4 * header->Frames);

    if (!vqabuf->Foff) {
        FreeBuffers(vqabuf, config);
        return NULL;
    }

    vqabuf->MemUsed += 4 * header->Frames;

    return vqabuf;
}

// done
void FreeBuffers(VQAData *vqa, VQAConfig *config)
{
    if (vqa->Foff) {
        free(vqa->Foff);
    }

    if (!config->ImageBuf && vqa->Drawer.ImageBuf) {
        free(vqa->Drawer.ImageBuf);
    }

    VQAFrameNode *frame_this = vqa->FrameData;

    for (int i = 0; i < config->NumFrameBufs && frame_this; ++i) {
        VQAFrameNode *frame_next = frame_this->Next;
        free(frame_this);
        frame_this = frame_next;
    }

    VQACBNode *cb_this = vqa->CBData;

    for (int i = 0; i < config->NumCBBufs && cb_this; ++i) {
        VQACBNode *cb_next = cb_this->Next;
        free(cb_this);
        cb_this = cb_next;
    }

    free(vqa);
}

// done
int32_t PrimeBuffers(VQAHandle *vqa)
{
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;

    for (int i = 0; i < config->NumFrameBufs; ++i) {
        int32_t rc = VQA_LoadFrame(vqa);
        if (!rc) {
            ++vqabuf->LoadedFrames;
        } else {
            if (rc != -8 && rc != -10) {
                return rc;
            }
        }
    }
    return 0;
}

// LOLPREVIEW
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CPLZ
// VPTK
// VPTR
// VPT0
// VPKZ
// VPTZ

// POLY MAC
// CBF0
// CBFZ
// CBP0
// CBPZ
// CPL0
// CBPZ
// VPT0
// VPTR
// VPRZ
// VPTD
// VPRZ

// todo
int32_t Load_VQF(VQAHandleP *vqap, uint32_t frame_iffsize)
{
    _ChunkHeader chunk;

    uint32_t bytes_loaded = 0;

    VQAData *vqabuf = vqap->VQABuf;
    VQAFrameNode *curframe = vqabuf->Loader.CurFrame;
    uint32_t framesize = (frame_iffsize + 1) & 0xFFFFFFFE;
    VQADrawer *drawer = &vqap->VQABuf->Drawer;

    if (framesize > 0) {
        do {
            if (vqap->StreamHandler(((VQAHandle *)vqap), 5, &chunk, sizeof(chunk))) {
                return -1;
            }
            uint32_t iffsize = (chunk.size << 24) & 0xFF000000 | (chunk.size << 8) & 0xFF0000 | (chunk.size >> 8) & 0xFF00
                | (chunk.size >> 24);
            bytes_loaded += ((iffsize + 1) & 0xFFFFFFFE) + 8;
#if 0
            if (chunk.id <= 'RTPV') {
                if (chunk.id == 'RTPV') {
                    if (Load_VPT0(vqap, iffsize)) {
                        return -3;
                    }
                    continue;
                }
                if (chunk.id <= '0TPV') {
                    switch (chunk.id) {
                        case '0TPV':
                            if (Load_VPT0(vqap, iffsize)) {
                                return -3;
                            }
                            break;
                        case '0FBC':
                            if (Load_CBF0(vqap, iffsize)) {
                                return -3;
                            }
                            break;
                        case '0LPC':
                            if (Load_CPL0(vqap, iffsize)) {
                                return -3;
                            }
                            if (!drawer->CurPalSize) {
                                memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
                                drawer->CurPalSize = curframe->PaletteSize;
                            }
                            curframe->Flags |= 4u;
                            break;
                        case '0PBC':
                            if (Load_CBP0(vqap, iffsize)) {
                                return -3;
                            }
                            break;
                        default:
                            return -3;
                    }
                } else {
                    if (chunk.id == 'DTPV') {
                        if (Load_VPTZ(vqap, iffsize)) {
                            return -3;
                        }
                        continue;
                    }
                    if (chunk.id != 'KTPV') {
                        return -3;
                    }
                    if (Load_VPTZ(vqap, iffsize)) {
                        return -3;
                    }
                    curframe->Flags |= 2u;
                }
            } else {
                if (chunk.id > 'ZPBC') {
                    if (chunk.id == 'ZRPV') {
                        if (Load_VPTZ(vqap, iffsize)) {
                            return -3;
                        }
                        continue;
                    }
                    if (chunk.id != 'ZTPV') {
                        return -3;
                    }
                    if (Load_VPTZ(vqap, iffsize)) {
                        return -3;
                    }
                    continue;
                }
                switch (chunk.id) {
                    case 'ZPBC':
                        if (Load_CBPZ(vqap, iffsize)) {
                            return -3;
                        }
                        break;
                    case 'ZFBC':
                        if (Load_CBFZ(vqap, iffsize)) {
                            return -3;
                        }
                        break;
                    case 'ZLPC':
                        if (Load_CPLZ(vqap, iffsize)) {
                            return -3;
                        }
                        if (!drawer->CurPalSize) {
                            drawer->CurPalSize = VQA_LCW_Uncompress(&curframe->Palette[curframe->PalOffset],
                                drawer->Palette_24,
                                vqabuf->Max_Pal_Size,
                                vqabuf->Max_Pal_Size - curframe->PalOffset);
                        }
                        curframe->Flags |= 4u;
                        break;
                    default:
                        return -3;
                }
            }
#endif
            switch (chunk.id) {
                case '0FBC':
                    if (Load_CBF0(vqap, iffsize)) {
                        return -3;
                    }
                    break;
                case 'ZFBC':
                    if (Load_CBFZ(vqap, iffsize)) {
                        return -3;
                    }
                    break;
                case '0PBC':
                    if (Load_CBP0(vqap, iffsize)) {
                        return -3;
                    }
                    break;
                case 'ZPBC':
                    if (Load_CBPZ(vqap, iffsize)) {
                        return -3;
                    }
                    break;
                case '0LPC':
                    if (Load_CPL0(vqap, iffsize)) {
                        return -3;
                    }
                    if (!drawer->CurPalSize) {
                        memcpy(drawer->Palette_24, curframe->Palette, curframe->PaletteSize);
                        drawer->CurPalSize = curframe->PaletteSize;
                    }
                    curframe->Flags |= 4;
                    break;
                case 'ZLPC':
                    if (Load_CPLZ(vqap, iffsize)) {
                        return -3;
                    }
                    if (!drawer->CurPalSize) {
                        drawer->CurPalSize = VQA_LCW_Uncompress(&curframe->Palette[curframe->PalOffset],
                            drawer->Palette_24,
                            vqabuf->Max_Pal_Size,
                            vqabuf->Max_Pal_Size - curframe->PalOffset);
                    }
                    curframe->Flags |= 4u;
                    break;
                    // fixme
            }

        } while (bytes_loaded < framesize);
    }
    return 0;
}

// done
int32_t Load_FINF(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAData *vqabuf = vqap->VQABuf;

    if (vqabuf->Foff) {
        if (vqap->StreamHandler((VQAHandle *)vqap, 5, vqabuf->Foff, (iffsize + 1) & 0xFFFFFFFE)) {
            return -3;
        }
    } else if (vqap->StreamHandler((VQAHandle *)vqap, 7, (void *)1, (iffsize + 1) & 0xFFFFFFFE)) {
        return -5;
    }
    return 0;
}

// done
int32_t Load_VQHD(VQAHandleP *vqap, uint32_t iffsize)
{
    if (vqap->StreamHandler((VQAHandle *)vqap, 5, &vqap->Header, (iffsize + 1) & 0xFFFFFFFE)) {
        return -3;
    }

    VQA_Configure_Drawer(vqap);
    return 0;
}

// done
int32_t Load_CBF0(VQAHandleP *vqap, uint32_t iffsize)
{
    VQALoader *loader = &vqap->VQABuf->Loader;
    VQACBNode *curcb = loader->CurCB;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, curcb->Buffer, (iffsize + 1) & 0xFFFFFFFE)) {
        return -3;
    }
    loader->NumPartialCB = 0;
    curcb->Flags &= ~2;
    curcb->CBOffset = 0;
    loader->FullCB = curcb;
    loader->FullCB->Flags &= ~1;
    loader->CurCB = curcb->Next;
    return 0;
}

// done
int32_t Load_CBFZ(VQAHandleP *vqap, uint32_t iffsize)
{
    VQALoader *loader = &vqap->VQABuf->Loader;
    VQACBNode *curcb = loader->CurCB;

    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;
    // todo check this cast in asm
    uint32_t lcwoffset = vqap->VQABuf->Max_CB_Size - (uint16_t)padsize;

    void *buffer = curcb->Buffer + lcwoffset;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, padsize)) {
        return -3;
    }

    loader->NumPartialCB = 0;
    curcb->Flags |= 2;
    curcb->CBOffset = lcwoffset;
    loader->FullCB = curcb;
    loader->FullCB->Flags &= ~1;
    loader->CurCB = curcb->Next;
    return 0;
}

// done
int32_t Load_CBP0(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAData *vqabuf = vqap->VQABuf;
    VQALoader *loader = &vqabuf->Loader;
    VQACBNode *curcb = loader->CurCB;

    void *buffer = curcb->Buffer + loader->PartialCBSize;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, (iffsize + 1) & 0xFFFFFFFE)) {
        return -3;
    }

    // todo check this cast in asm
    loader->PartialCBSize += (uint16_t)iffsize;

    if (vqap->Header.Groupsize != ++loader->NumPartialCB) {
        return 0;
    }

    loader->NumPartialCB = 0;
    loader->PartialCBSize = 0;
    curcb->Flags &= ~2;
    curcb->CBOffset = 0;
    loader->FullCB = curcb;
    loader->FullCB->Flags &= ~1;
    loader->CurCB = curcb->Next;
    return 0;
}

// done
int32_t Load_CBPZ(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAData *vqabuf = vqap->VQABuf;
    VQALoader *loader = &vqabuf->Loader;
    VQACBNode *curcb = loader->CurCB;

    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;

    if (!loader->PartialCBSize) {
        // todo check this cast in asm
        curcb->CBOffset = vqabuf->Max_CB_Size - (vqap->Header.Groupsize * (uint16_t)padsize + 100);
    }
    void *buffer = curcb->Buffer + curcb->CBOffset + loader->PartialCBSize;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, padsize)) {
        return -3;
    }

    // todo check this cast in asm
    loader->PartialCBSize += (uint16_t)iffsize;

    if (vqap->Header.Groupsize == ++loader->NumPartialCB) {
        loader->NumPartialCB = 0;
        loader->PartialCBSize = 0;
        curcb->Flags |= 2;
        loader->FullCB = curcb;
        loader->FullCB->Flags &= ~1;
        loader->CurCB = curcb->Next;
    }

    return 0;
}

// done
int32_t Load_CPL0(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAFrameNode *curframe = vqap->VQABuf->Loader.CurFrame;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, curframe->Palette, (iffsize + 1) & 0xFFFFFFFE)) {
        return -3;
    }

    curframe->Flags &= ~8;
    curframe->PalOffset = 0;
    // todo check this cast in asm
    curframe->PaletteSize = (uint16_t)iffsize;
    return 0;
}

// done
int32_t Load_CPLZ(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAFrameNode *curframe = vqap->VQABuf->Loader.CurFrame;
    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;

    // todo check this cast in asm
    uint32_t lcwoffset = vqap->VQABuf->Max_Pal_Size - (uint16_t)padsize;

    void *buffer = curframe->Palette + lcwoffset;
    if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, padsize)) {
        return -3;
    }

    curframe->Flags |= 8;
    curframe->PalOffset = lcwoffset;
    // todo check this cast in asm
    curframe->PaletteSize = (uint16_t)iffsize;
    return 0;
}

// done
int32_t Load_VPT0(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAFrameNode *curframe = vqap->VQABuf->Loader.CurFrame;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, curframe->Pointers, (iffsize + 1) & 0xFFFFFFFE)) {
        return -3;
    }

    curframe->Flags &= ~16;
    curframe->PtrOffset = 0;
    return 0;
}

// done
int32_t Load_VPTZ(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAFrameNode *curframe = vqap->VQABuf->Loader.CurFrame;
    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;
    // todo check this cast in asm
    uint32_t lcwoffset = vqap->VQABuf->Max_Ptr_Size - (uint16_t)padsize;
    void *buffer = curframe->Pointers + lcwoffset;

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, padsize)) {
        return -3;
    }
    curframe->Flags |= 16;
    curframe->PtrOffset = lcwoffset;
    return 0;
}

#if 0
int32_t Load_SND0(VQAHandleP *vqap, uint32_t iffsize)
{
    int32_t rc;

    VQAConfig *config = &vqap->Config;
    VQAData *vqabuf = vqap->VQABuf;
    VQAAudio *audio = &vqabuf->Audio;

    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;

    if (!(config->OptionFlags & 1) || !audio->Buffer) {
        rc = vqap->StreamHandler((VQAHandle *)vqap, 7, (void *)1, padsize);
        if (rc) {
            rc = -5;
        }
        return rc;
    }

    if (padsize > audio->TempBufLen && !audio->AudBufPos) {
        if (vqap->StreamHandler((VQAHandle *)vqap, 5, audio->Buffer, padsize)) {
            return -3;
        }
        int i = 0;
        int j = 0;
        audio->AudBufPos += iffsize;
        while (i < iffsize / config->HMIBufSize) {
            ++i;
            audio->IsLoaded[j] = 1;
            ++j;
        }
        return 0;
    }

    rc = vqap->StreamHandler((VQAHandle *)vqap, 5, audio->TempBuf, padsize);
    if (rc) {
        rc = -3;
    } else {
        audio->TempBufSize = iffsize;
    }
    return rc;
}

int32_t Load_SND1(VQAHandleP *vqap, uint32_t iffsize)
{
    int32_t rc;
    VQASND1Header snd1header;

    _VQAConfig *config = &vqap->Config;
    _VQAAudio *audio = &vqap->VQABuf->Audio;
    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;

    if (!(vqap->Config.OptionFlags & 1) || !audio->Buffer) {
        rc = vqap->StreamHandler((VQAHandle *)vqap, 7, (void *)1, padsize);
        if (rc) {
            rc = -5;
        }
        return rc;
    }

    if (vqap->StreamHandler((VQAHandle *)vqap, 5, &snd1header, sizeof(VQASND1Header))) {
        return -3;
    }

    padsize -= sizeof(VQASND1Header);

    if (snd1header.OutSize > audio->TempBufLen && !audio->AudBufPos) {
        if (snd1header.OutSize == snd1header.Size) {
            if (vqap->StreamHandler((VQAHandle *)vqap, 5, audio->Buffer, padsize)) {
                return -3;
            }
        } else {
            void *buffer = audio->Buffer + (config->AudioBufSize - padsize);
            if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, padsize)) {
                return -3;
            }
            AudioUnzap(buffer, audio->Buffer, snd1header.OutSize);
        }

        int i = 0;
        int j = 0;
        audio->AudBufPos += snd1header.OutSize;
        while (i < snd1header.OutSize / config->HMIBufSize) {
            ++i;
            audio->IsLoaded[j] = 1;
            ++j;
        }
        return 0;
    }

    if (snd1header.OutSize == snd1header.Size) {
        if (vqap->StreamHandler((VQAHandle *)vqap, 5, audio->TempBuf, padsize)) {
            return -3;
        }
    } else {
        void *buffer = audio->TempBuf + (audio->TempBufLen - padsize);
        if (vqap->StreamHandler((VQAHandle *)vqap, 5, buffer, padsize)) {
            return -3;
        }

        AudioUnzap(buffer, audio->TempBuf, snd1header.OutSize);
    }
    audio->TempBufSize = snd1header.OutSize;

    return 0;
}

int32_t Load_SND2(VQAHandleP *vqap, uint32_t iffsize)
{
    VQAErrorType rc;

    _VQAAudio *audio = &vqap->VQABuf->Audio;
    _VQAConfig *config = &vqap->Config;
    uint32_t padsize = (iffsize + 1) & 0xFFFFFFFE;

    if (!(vqap->Config.OptionFlags & 1) || !audio->Buffer) {
        rc = vqap->StreamHandler((VQAHandle *)vqap, 7, (void *)1, padsize);
        if (rc) {
            rc = -5;
        }
        return rc;
    }

    //fixme
    uint32_t size = ((vqap->VQABuf->Audio.BitsPerSample
                         - (__CFSHL__((signed int)vqap->VQABuf->Audio.BitsPerSample >> 31, 2)
                             + 4 * ((signed int)vqap->VQABuf->Audio.BitsPerSample >> 31)))
            >> 2)
        * iffsize;
    if (size > vqap->VQABuf->Audio.TempBufLen && !vqap->VQABuf->Audio.AudBufPos) {
        void *buffer = audio->Buffer + (vqap->Config.AudioBufSize - padsize);
        if (vqap->StreamHandler(vqap, 5, buffer, padsize)) {
            return -3;
        }
        audio->sSOSInfo.lpSource = buffer;
        audio->sSOSInfo.lpDest = audio->Buffer;
        VQA_sosCODECDecompressData(&audio->sSOSInfo, size);
        audio->AudBufPos += size;
        int i = 0;
        int j = 0;
        while (i < size / config->HMIBufSize) {
            ++i;
            audio->IsLoaded[j] = 1;
            ++j;
        }
        return 0;
    }

    void *buffer = vqap->VQABuf->Audio.TempBuf + (vqap->VQABuf->Audio.TempBufLen - padsize);
    if (vqap->StreamHandler(vqap, 5, buffer, padsize)) {
        return -3;
    }

    audio->sSOSInfo.lpSource = buffer;
    audio->sSOSInfo.lpDest = audio->TempBuf;
    VQA_sosCODECDecompressData(&audio->sSOSInfo, size);
    audio->TempBufSize = size;

    return 0;
}
#endif