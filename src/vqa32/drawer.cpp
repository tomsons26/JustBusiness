#include "vqaplay.h"
#include <cstring>

void PageFlip_Nop(VQAHandle *vqa);
void UnVQ_2x2(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
void UnVQ_Nop(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
void UnVQ_2x3(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
void UnVQ_4x4_REAL(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
void UnVQ_4x2(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
void UnVQ_4x4_NEW(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth);
void PageFlip_MCGA(VQAHandle *vqa);
void PageFlip_MCGABuf(VQAHandle *vqa);
int32_t DrawFrame_MCGA(VQAHandle *vqa);
int32_t DrawFrame_MCGABuf(VQAHandle *vqa);
int32_t DrawFrame_Buffer(VQAHandle *vqa);
uint32_t VQA_GetTime();
int32_t VQA_LCW_Uncompress(uint8_t *str, uint8_t *palette, int32_t int32, int32_t int33);
void WaitNoVB(int32_t int32);
void WaitVB(int32_t int32);
void SetPalette(uint8_t *pal, int32_t int32, int slowpal);
void MCGA_Blit(uint8_t *buf, uint8_t *scrn, int32_t int32, int32_t imgheight);
void MCGA_BufferCopy(uint8_t *buf);

// done
char *VQA_GetPalette(VQAHandle *vqa)
{
    char *palette = NULL;
    VQADrawer *drawer = &((VQAHandleP *)vqa)->VQABuf->Drawer;
    if (drawer->CurPalSize > 0) {
        palette = (char *)drawer->Palette_24;
    }
    return palette;
}

// done
int32_t VQA_GetPaletteSize(VQAHandle *vqa)
{
    VQADrawer *drawer = &((VQAHandleP *)vqa)->VQABuf->Drawer;

    return drawer->CurPalSize;
}

// done
void VQA_Set_DrawBuffer(VQAHandle *vqa, uint8_t *buffer, uint32_t width, uint32_t height, int32_t xpos, int32_t ypos)
{
    VQADrawer *drawer = &((VQAHandleP *)vqa)->VQABuf->Drawer;
    VQAHeader *header = &((VQAHandleP *)vqa)->Header;
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;

    int32_t origin = config->DrawFlags & 48;
    drawer->ImageBuf = buffer;
    drawer->ImageWidth = width;
    drawer->ImageHeight = height;

    if (xpos == -1 && ypos == -1) {
        drawer->X1 = (drawer->ImageWidth - header->ImageWidth) / 2;
        drawer->Y1 = (drawer->ImageHeight - header->ImageHeight) / 2;
        drawer->X2 = drawer->X1 + header->ImageWidth - 1;
        drawer->Y2 = drawer->Y1 + header->ImageHeight - 1;
        // switch?
    } else if (origin != 32) {
        if (origin < 32 || origin != 48) {
            drawer->X1 = xpos;
            drawer->Y1 = ypos;
            drawer->X2 = drawer->X1 + header->ImageWidth - 1;
            drawer->Y2 = drawer->Y1 + header->ImageHeight - 1;
        } else {
            drawer->X1 = xpos;
            drawer->Y1 = drawer->ImageHeight - ypos;
            drawer->X2 = drawer->X1 + header->ImageWidth - 1;
            drawer->Y2 = drawer->Y2 - header->ImageHeight - 1;
        }
    } else {
        drawer->X1 = drawer->ImageWidth - xpos;
        drawer->Y1 = drawer->ImageHeight - ypos;
        drawer->X2 = drawer->X1 - header->ImageWidth;
        drawer->Y2 = drawer->Y1 - header->ImageHeight;
    }

    drawer->ScreenOffset = drawer->X1 + drawer->Y1 * drawer->ImageWidth;
}

// done
void VQA_Configure_Drawer(VQAHandleP *vqap)
{
    VQAConfig *config = &vqap->Config;
    VQAData *vqabuf = vqap->VQABuf;
    VQAHeader *header = &vqap->Header;
    int32_t origin = vqap->Config.DrawFlags & 48;

    if (config->X1 && config->Y1 == -1) {
        vqabuf->Drawer.X1 = (vqabuf->Drawer.ImageWidth - header->ImageWidth) / 2;
        vqabuf->Drawer.Y1 = (vqabuf->Drawer.ImageHeight - header->ImageHeight) / 2;
        vqabuf->Drawer.X2 = vqabuf->Drawer.X1 + header->ImageWidth - 1;
        vqabuf->Drawer.Y2 = vqabuf->Drawer.Y1 + header->ImageHeight - 1;

        // switch?
    } else if (origin != 32) {
        if (origin < 32 || origin != 48) {
            vqabuf->Drawer.X1 = config->X1;
            vqabuf->Drawer.Y1 = config->Y1;
            vqabuf->Drawer.X2 = vqabuf->Drawer.X1 + header->ImageWidth - 1;
            vqabuf->Drawer.Y2 = vqabuf->Drawer.Y1 + header->ImageHeight - 1;
        } else {
            vqabuf->Drawer.X1 = config->X1;
            vqabuf->Drawer.Y1 = vqabuf->Drawer.ImageHeight - config->Y1;
            vqabuf->Drawer.X2 = vqabuf->Drawer.X1 + header->ImageWidth - 1;
            vqabuf->Drawer.Y2 = vqabuf->Drawer.Y2 - header->ImageHeight - 1;
        }
    } else {
        vqabuf->Drawer.X1 = vqabuf->Drawer.ImageWidth - config->X1;
        vqabuf->Drawer.Y1 = vqabuf->Drawer.ImageHeight - config->Y1;
        vqabuf->Drawer.X2 = vqabuf->Drawer.X1 - header->ImageWidth;
        vqabuf->Drawer.Y2 = vqabuf->Drawer.Y1 - header->ImageHeight;
    }

    vqabuf->Drawer.BlocksPerRow = header->ImageWidth / (int)header->BlockWidth;
    vqabuf->Drawer.NumRows = header->ImageHeight / (int)header->BlockHeight;
    vqabuf->Drawer.NumBlocks = vqabuf->Drawer.BlocksPerRow * vqabuf->Drawer.NumRows;

    int32_t blkdim = (header->BlockWidth << 8) | header->BlockHeight;

    vqabuf->UnVQ = UnVQ_Nop;
    vqabuf->Page_Flip = PageFlip_Nop;

    if (config->DrawFlags & 1) {
        if (origin < 1 || origin != 48) {
            switch (blkdim) {
                case 0x202:
                    vqabuf->UnVQ = UnVQ_2x2;
                    break;
                case 0x203:
                    vqabuf->UnVQ = UnVQ_2x3;
                    break;
                case 0x402:
                    vqabuf->UnVQ = UnVQ_4x2;
                    break;
                case 0x404:
                    vqabuf->UnVQ = UnVQ_4x4_REAL;
                    break;
            }
        } else {
            switch (blkdim) {
                case 0x202:
                    vqabuf->UnVQ = UnVQ_2x2;
                    break;
                case 0x203:
                    vqabuf->UnVQ = UnVQ_2x3;
                    break;
                case 0x402:
                    vqabuf->UnVQ = UnVQ_4x2;
                    break;
                case 0x404:
                    vqabuf->UnVQ = UnVQ_4x4_NEW;
                    break;
            }
        }
    }

    if (vqap->Config.Vmode == 19) {
        if (vqap->Config.DrawFlags & 1) {
            vqabuf->Draw_Frame = DrawFrame_MCGABuf;
            vqabuf->Page_Flip = PageFlip_MCGABuf;
        } else {
            vqabuf->Draw_Frame = DrawFrame_MCGA;
            vqabuf->Page_Flip = PageFlip_MCGA;

            switch (blkdim) {
                case 0x202:
                    vqabuf->UnVQ = UnVQ_2x2;
                    break;
                case 0x203:
                    vqabuf->UnVQ = UnVQ_2x3;
                    break;
                case 0x402:
                    vqabuf->UnVQ = UnVQ_4x2;
                    break;
                case 0x404:
                    vqabuf->UnVQ = UnVQ_4x4_REAL;
                    break;
            }
        }
        vqabuf->Drawer.ScreenOffset = vqabuf->Drawer.X1 + vqabuf->Drawer.Y1 * 320;
    } else {
        vqabuf->Draw_Frame = DrawFrame_Buffer;
        // mac
        // vqabuf->Page_Flip = PageFlip_Buffer;
        vqabuf->Drawer.ScreenOffset = vqabuf->Drawer.X1 + vqabuf->Drawer.Y1 * vqabuf->Drawer.ImageWidth;
    }
}

// done
unsigned int Select_Frame(VQAHandleP *vqap)
{
    VQAConfig *config = &vqap->Config;
    VQAData *vqabuf = vqap->VQABuf;
    VQAFrameNode *curframe = vqabuf->Drawer.CurFrame;

    if (!(curframe->Flags & 1)) {
        ++vqabuf->Drawer.WaitsOnLoader;
        return -8;
    }

    if (vqap->Config.OptionFlags & 2) {
        vqabuf->Drawer.LastFrame = curframe->FrameNum;
        return 0;
    }

    int32_t curtime = VQA_GetTime();
    int32_t desiredframe = curtime * vqap->Config.DrawRate / 60;

    vqabuf->Drawer.DesiredFrame = desiredframe;

    if (vqap->Config.DrawRate == vqap->Config.FrameRate) {
        if (curframe->FrameNum > desiredframe) {
            return -9;
        }
    } else if (60 / vqap->Config.DrawRate > curtime - vqabuf->Drawer.LastTime) {
        return -9;
    }

    if (vqap->Config.FrameRate / 5 <= curframe->FrameNum - vqabuf->Drawer.LastFrame) {
        vqabuf->Drawer.LastFrame = curframe->FrameNum;
        return 0;
    }

    if (vqap->Config.DrawFlags & 4) {
        vqabuf->Drawer.LastFrame = curframe->FrameNum;
        return 0;
    }

    while (true) {
        if (!(curframe->Flags & 1)) {
            return -8;
        }

        if (curframe->Flags & 2 || curframe->FrameNum >= desiredframe) {
            break;
        }

        if (curframe->Flags & 4) {
            if (curframe->Flags & 8) {
                curframe->PaletteSize = VQA_LCW_Uncompress(&curframe->Palette[curframe->PalOffset],
                    curframe->Palette,
                    vqabuf->Max_Pal_Size,
                    vqabuf->Max_Pal_Size - curframe->PalOffset);
                curframe->Flags &= ~8;
            }
            memcpy(vqabuf->Drawer.Palette_24, curframe->Palette, curframe->PaletteSize);
            vqabuf->Drawer.CurPalSize = curframe->PaletteSize;
            vqabuf->Drawer.Flags |= 1;
        }

        if (config->DrawerCallback) {
            config->DrawerCallback(NULL, curframe->FrameNum);
        }

        curframe->Flags = 0;
        curframe = curframe->Next;
        vqabuf->Drawer.CurFrame = curframe;
        ++vqabuf->Drawer.NumSkipped;
    }

    vqabuf->Drawer.LastFrame = curframe->FrameNum;
    vqabuf->Drawer.LastTime = curtime;

    return 0;
}

// done
void Prepare_Frame(VQAData *vqabuf)
{
    VQADrawer *drawer = &vqabuf->Drawer;
    VQAFrameNode *curframe = drawer->CurFrame;
    VQACBNode *codebook = drawer->CurFrame->Codebook;

    if (codebook->Flags & 2) {
        VQA_LCW_Uncompress(&codebook->Buffer[codebook->CBOffset],
            codebook->Buffer,
            vqabuf->Max_CB_Size,
            vqabuf->Max_CB_Size - codebook->CBOffset);
        // mac
        // for (int i = 0; i < vqabuf->Max_CB_Size; ++i) {
        //    codebook->Buffer[i] = vqa_codebook[codebook->Buffer[i]];
        //}
        codebook->Flags &= ~2;
    }

    if (curframe->Flags & 8) {
        curframe->PaletteSize = VQA_LCW_Uncompress(&curframe->Palette[curframe->PalOffset],
            curframe->Palette,
            vqabuf->Max_Pal_Size,
            vqabuf->Max_Pal_Size - curframe->PalOffset);
        curframe->Flags &= ~8;
    }

    if (curframe->Flags & 16) {
        VQA_LCW_Uncompress(&curframe->Pointers[curframe->PtrOffset],
            curframe->Pointers,
            vqabuf->Max_Ptr_Size,
            vqabuf->Max_Ptr_Size - curframe->PtrOffset);
        curframe->Flags &= ~16;
    }
}

// done
int32_t DrawFrame_MCGA(VQAHandle *vqa)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;
    if (!(vqabuf->Flags & 2)) {
        int32_t rc = Select_Frame((VQAHandleP *)vqa);
        if (rc) {
            return rc;
        }

        Prepare_Frame(vqabuf);
    }
    if (vqabuf->Flags & 1) {
        vqabuf->Flags |= 2;
        return -10;
    }

    if (vqabuf->Flags & 2) {
        ++vqabuf->Drawer.WaitsOnFlipper;
        vqabuf->Flags &= ~2;
    }

    VQAFrameNode *curframe = drawer->CurFrame;
    vqabuf->Drawer.LastFrameNum = drawer->CurFrame->FrameNum;
    vqabuf->Flipper.CurFrame = curframe;
    vqabuf->Flags |= 1;
    drawer->CurFrame = curframe->Next;

    return 0;
}

// done
void PageFlip_MCGA(VQAHandle *vqa)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;
    VQAFrameNode *curframe = vqabuf->Flipper.CurFrame;

    if (curframe->Flags & 4 || drawer->Flags & 1) {
        uint8_t *pal = curframe->Palette;
        int32_t palsize = curframe->PaletteSize;

        int slowpal = 1;
        if (!(config->OptionFlags & 16)) {
            slowpal = 0;
        }

        WaitNoVB(vqabuf->VBIBit);
        WaitVB(vqabuf->VBIBit);

        if (curframe->Flags & 4) {
            SetPalette(pal, palsize, slowpal);
        } else if (drawer->Flags & 1) {
            SetPalette(drawer->Palette_24, drawer->CurPalSize, slowpal);
            drawer->Flags &= ~1;
        }
    }

    uint8_t *cb = curframe->Codebook->Buffer;
    uint8_t *ptrs = curframe->Pointers;
    uint8_t *scrn = (uint8_t *)(0xA0000 + drawer->ScreenOffset);
    int32_t blocksperrow = drawer->BlocksPerRow;
    int32_t numrows = drawer->NumRows;

    vqabuf->UnVQ(cb, ptrs, scrn, blocksperrow, numrows, 320);

    if (config->DrawerCallback) {
        config->DrawerCallback(NULL, curframe->FrameNum);
    }
}

// done
int32_t DrawFrame_MCGABuf(VQAHandle *vqa)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;

    if (!(vqabuf->Flags & 2)) {
        int32_t rc = Select_Frame((VQAHandleP *)vqa);
        if (rc) {
            return rc;
        }
        Prepare_Frame(vqabuf);
    }

    if (vqabuf->Flags & 1) {
        vqabuf->Flags |= 2;
        return -10;
    }

    if (vqabuf->Flags & 2) {
        ++vqabuf->Drawer.WaitsOnFlipper;
        vqabuf->Flags &= ~2;
    }

    VQAFrameNode *curframe = drawer->CurFrame;

    uint8_t *scrn = vqabuf->Drawer.ImageBuf;

    vqabuf->UnVQ(drawer->CurFrame->Codebook->Buffer,
        drawer->CurFrame->Pointers,
        scrn,
        vqabuf->Drawer.BlocksPerRow,
        vqabuf->Drawer.NumRows,
        ((VQAHandleP *)vqa)->Header.ImageWidth);

    vqabuf->Drawer.LastFrameNum = curframe->FrameNum;
    vqabuf->Flipper.CurFrame = curframe;
    vqabuf->Flags |= 1;
    drawer->CurFrame = curframe->Next;

    return 0;
}

// done
void PageFlip_MCGABuf(VQAHandle *vqa)
{
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;
    VQAFrameNode *curframe = vqabuf->Flipper.CurFrame;

    uint8_t *pal = curframe->Palette;
    int32_t palsize = curframe->PaletteSize;

    int slowpal = 1;
    if (!(config->OptionFlags & 16)) {
        slowpal = 0;
    }

    if (curframe->Flags & 4) {
        WaitNoVB(vqabuf->VBIBit);
        WaitVB(vqabuf->VBIBit);
        SetPalette(pal, palsize, slowpal);
    } else if (drawer->Flags & 1) {
        WaitNoVB(vqabuf->VBIBit);
        WaitVB(vqabuf->VBIBit);
        SetPalette(drawer->Palette_24, drawer->CurPalSize, slowpal);
        drawer->Flags &= ~1;
    }

    int32_t imgwidth = ((VQAHandleP *)vqa)->Header.ImageWidth;
    int32_t imgheight = ((VQAHandleP *)vqa)->Header.ImageHeight;

    uint8_t *buf = drawer->ImageBuf;

    uint8_t *scrn = (uint8_t *)(0xA0000 + drawer->ScreenOffset);

    if (imgwidth < 320 || imgheight < 200) {
        MCGA_Blit(buf, scrn, imgwidth, imgheight);
    } else {
        MCGA_BufferCopy(buf);
    }

    if (config->DrawerCallback) {
        config->DrawerCallback(NULL, curframe->FrameNum);
    }
}

// done
int32_t DrawFrame_Buffer(VQAHandle *vqa)
{
    VQAConfig *config = &((VQAHandleP *)vqa)->Config;
    VQAData *vqabuf = ((VQAHandleP *)vqa)->VQABuf;
    VQADrawer *drawer = &vqabuf->Drawer;

    if (!(vqabuf->Flags & 2)) {
        int32_t rc = Select_Frame((VQAHandleP *)vqa);
        if (rc) {
            return rc;
        }
        Prepare_Frame(vqabuf);
    }
    if (vqabuf->Flags & 1) {
        vqabuf->Flags |= 2;
        return -10;
    }

    if (vqabuf->Flags & 2) {
        ++drawer->WaitsOnFlipper;
        vqabuf->Flags &= ~2;
    }

    VQAFrameNode *curframe = drawer->CurFrame;

    uint8_t *buff = drawer->ImageBuf + drawer->ScreenOffset;

    vqabuf->UnVQ(drawer->CurFrame->Codebook->Buffer,
        drawer->CurFrame->Pointers,
        buff,
        drawer->BlocksPerRow,
        drawer->NumRows,
        drawer->ImageWidth);

    drawer->LastFrameNum = curframe->FrameNum;
    vqabuf->Flipper.CurFrame = curframe;
    vqabuf->Flags |= 1;

    if (config->DrawerCallback) {
        config->DrawerCallback(drawer->ImageBuf, curframe->FrameNum);
    }

    drawer->CurFrame = curframe->Next;

    return 0;
}

void UnVQ_Nop(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth)
{
}

void PageFlip_Nop(VQAHandle *vqa) {}
