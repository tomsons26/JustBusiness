#include "vqa32/vqaplay.h"

#include <cstdint>

// temp dummies
void UnVQ_2x3(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth)
{
}
void UnVQ_4x4_REAL(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth)
{
}
void UnVQ_4x2(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth)
{
}
void UnVQ_4x4_NEW(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth)
{
}

void UnVQ_2x2(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, uint32_t blocksperrow, uint32_t numrows, uint32_t bufwidth)
{
}

int32_t VQA_LCW_Uncompress(uint8_t *str, uint8_t *palette, int32_t int32, int32_t int33)
{
    return 0;
}
void WaitNoVB(int32_t int32) {}
void WaitVB(int32_t int32) {}
void SetPalette(uint8_t *pal, int32_t int32, int slowpal) {}
void MCGA_Blit(uint8_t *buf, uint8_t *scrn, int32_t int32, int32_t imgheight){}
void MCGA_BufferCopy(uint8_t *buf){}
int32_t TestVBIBit(){return 0;}

int32_t Check_Key()
{
    return 0;
}
void Get_Key(){}

void VQA_InitMono(VQAHandleP *vqap)
{}
void VQA_UpdateMono(VQAHandleP *vqap)
{}

int32_t Load_PMT_Data_File(char *str)
{
    return 0;
}