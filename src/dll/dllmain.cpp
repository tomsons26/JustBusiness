/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Provides the exe patching/hooking entry point for the proxy loader.
 *
 * @copyright JustBusiness is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#ifndef DLL_BUILD
#error "This file should only be compiled for dll injection."
#endif

#include <corecrt_io.h>
#include <cstdint>
#include <cstdio>
#include <hooker.h>
#include <windows.h>

#include <memoryapi.h>
#include <processthreadsapi.h>

// For the few locations where a hook needs to be in the code, this macro can list all addresses and will select
// the correct one at compile time.
#ifdef BUILD_EDITOR
#define PICK_ADDRESS(a, b) (b)
#else
#define PICK_ADDRESS(a, b) (a)
#endif

// Helper Functions based on those from OpenMC2

// Use Make_Global to access global variables in the original exe once you know
// the correct type and address. This should not change from run to run if the
// exe loads at a standard base address.

// Typical use will be to use define to create a friendly name, e.g:
// #define SomeGlobalVar (Make_Global<bool>(0x00FF00FF))
// allows you to use SomeGlobalVar as though it was a bool you declared, though
// it will reflect the value the original exe sees at address 0x00FF00FF
template<typename T>
__forceinline T &Make_Global(const uintptr_t address)
{
    return *reinterpret_cast<T *>(address);
}

template<typename T>
__forceinline T *Make_Pointer(const uintptr_t address)
{
    return reinterpret_cast<T *>(address);
}

// Call_Function and Call_Method can be used to call functions from the original
// binary where they are required and a replacement hasn't been written yet.
template<typename ReturnType, typename... Arguments>
__forceinline ReturnType Call_Function(const uintptr_t address, Arguments... args)
{
    return reinterpret_cast<ReturnType(__cdecl *)(Arguments...)>(address)(args...);
}

template<typename ReturnType, typename... Arguments>
__forceinline ReturnType Call__StdCall_Function(const uintptr_t address, Arguments... args)
{
    return reinterpret_cast<ReturnType(__stdcall *)(Arguments...)>(address)(args...);
}

template<typename ReturnType, typename ThisType, typename... Arguments>
__forceinline ReturnType Call_Method(const uintptr_t address, ThisType *const self, Arguments... args)
{
    return reinterpret_cast<ReturnType(__thiscall *)(ThisType *, Arguments...)>(address)(self, args...);
}

// These create pointers to various types of function where the return,
// parameters and calling convention are known.
template<typename ReturnType, typename... Arguments>
__forceinline ReturnType(__cdecl *Make_Function_Ptr(const uintptr_t address))(Arguments...)
{
    return reinterpret_cast<ReturnType(__cdecl *)(Arguments...)>(address);
}

template<typename ReturnType, typename... Arguments>
__forceinline ReturnType(__stdcall *Make_StdCall_Ptr(const uintptr_t address))(Arguments...)
{
    return reinterpret_cast<ReturnType(__stdcall *)(Arguments...)>(address);
}

template<typename ReturnType, typename ThisType, typename... Arguments>
__forceinline ReturnType(__thiscall *Make_Method_Ptr(const uintptr_t address))(ThisType *, Arguments...)
{
    return reinterpret_cast<ReturnType(__thiscall *)(ThisType *, Arguments...)>(address);
}

template<typename ReturnType, typename... Arguments>
__forceinline ReturnType(__cdecl *Make_VA_Function_Ptr(const uintptr_t address))(Arguments..., ...)
{
    return reinterpret_cast<ReturnType(__cdecl *)(Arguments..., ...)>(address);
}

// A nice struct to pack the assembly in for jumping into replacement code.
// So long as the calling conventions and arguments for the replaced and
// replacement functions are the same, everything should just work.
#pragma pack(push, 1)
struct x86_jump
{
    const uint8_t cmd = 0xE9;
    uintptr_t addr;
};
#pragma pack(pop)

// Use these to hook existing functions and replace them with newly written ones
// to either replace them permanently or just to test correctness of a newly
// written one.

// Base hooking function to apply the actual hook.
inline void Hook_Func(uintptr_t in, uintptr_t out)
{
#ifdef DLL_BUILD
    static_assert(sizeof(x86_jump) == 5, "Jump struct not expected size.");

    x86_jump cmd;
    cmd.addr = out - in - 5;
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)in, &cmd, 5, nullptr);
#endif
}

// Hook regular functions and static methods.
template<typename T>
void Hook_Function(uintptr_t in, T out)
{
    Hook_Func(in, reinterpret_cast<uintptr_t>(out));
}

// Method pointers need funky syntax to get the underlying function address
// hence the odd cast to void for the out pointer.
template<typename T>
void Hook_Method(uintptr_t in, T out)
{
    Hook_Func(in, reinterpret_cast<uintptr_t>((void *&)out));
}

// Virtuals are even trickier so resort to inline assembler for those courtesy of the TTScripts guys.
#define Hook_Any(in, out) __asm { \
      __asm push out                  \
      __asm push in                         \
      __asm call Hook_Func     \
      __asm add esp, 8}

#define ARRAY_DEC(type, var, size) type(&var)[size]
#define ARRAY_DEF(address, type, var, size) type(&var)[size] = Make_Global<type[size]>(address);
#define ARRAY2D_DEC(type, var, x, y) type(&var)[x][y]
#define ARRAY2D_DEF(address, type, var, x, y) type(&var)[x][y] = Make_Global<type[x][y]>(address);

int &DIB_Bottom_Up = Make_Global<int>(0x004462B4);
unsigned int &Monopoly_Start_Time = Make_Global<unsigned int>(0x0047ABB0);
HPALETTE &Main_Palette_Handle = Make_Global<HPALETTE>(0x004466F0);
HDC &WinG_DC = Make_Global<HDC>(0x00481890);
HWND &Main_Window_Handle = Make_Global<HWND>(0x004466E8);
MSG &Global_Message_Queue = Make_Global<MSG>(0x00481868);

int &Is_Palette_Device = Make_Global<int>(0x004462A8);
int &Is_Supported_Depth = Make_Global<int>(0x00446264);
int &System_Screen_Width = Make_Global<int>(0x0047AB50);
int &System_Screen_Height = Make_Global<int>(0x0047AB54);
int &System_Border_Width = Make_Global<int>(0x0047AB58);
int &System_Border_Height = Make_Global<int>(0x0047AB5C);
int &System_Caption_Height = Make_Global<int>(0x0047AB60);
int &System_Menu_Bar_Height = Make_Global<int>(0x0047AB64);

ARRAY_DEF(0x0047ABB4, char, Text_String, 255);
ARRAY_DEF(0x00446468, char, Executable_Path, 200);
ARRAY_DEF(0x00446637, char, CD_Root_Path, 200);

char test[100];
int &___dll_argc = Make_Global<int>(0x00483864);
char **&__dll_argv = Make_Global<char **>(0x00483868);

void Get_Executable_And_CD_Paths1()
{
    memset(test, 0, sizeof(test));

    char *ptr;

    if (___dll_argc >= 1 && strlen(*__dll_argv) < 200) {
        strcpy(Text_String, *__dll_argv);
        for (ptr = &Text_String[strlen(Text_String) - 1]; *ptr != '\\'; --ptr) {
            ;
        }
        ptr[1] = 0;
        strcpy(Executable_Path, Text_String);
    }

    char poly_path[260];
    signed int i = 'A';
    do {
        sprintf(Text_String, "%c:\\", i);
        if (GetDriveTypeA(Text_String) == 5) {
            sprintf(poly_path, "%c:\\MONOPOLY\\MONOPOLY.EXE", i);
            if (access(poly_path, 0) != -1) {
                strcpy(CD_Root_Path, Text_String);
                strcat(CD_Root_Path, "MONOPOLY\\");
                sprintf(test, "somehing in %c, setting z", (char)i);
                MessageBox(0, test, "aaa", 0);
                i = 'Z';
            }

        } else {
        }
        ++i;
    } while (i <= 'Z');
}

typedef unsigned int _DWORD;
typedef unsigned short _WORD;
typedef unsigned char _BYTE;
#define dont_byteswap_ushort(x) (x)

void __cdecl UnVQ4x4(unsigned __int8 *codebook, unsigned __int8 *pointers, unsigned __int8 *buffer,
    unsigned int blocksperrow, unsigned int numrows, unsigned int bufwidth)
{
    unsigned __int8 *v6; // r9
    int v7; // r10
    int v8; // r28
    int v9; // r0
    unsigned int v10; // r29
    unsigned __int16 v11; // r8
    int v12; // r11
    unsigned __int16 n; // r12
    unsigned __int16 ii; // r11
    unsigned __int8 *v15; // r8
    unsigned __int16 jj; // r12
    unsigned __int16 kk; // r11
    int v18; // r30
    __int16 i; // r7
    unsigned __int16 v20; // r8
    unsigned __int16 j; // r12
    unsigned __int16 k; // r11
    unsigned __int8 *v23; // r5
    unsigned __int16 l; // r12
    unsigned __int16 m; // r11
    unsigned __int8 v26; // r31
    int v27; // r30
    unsigned __int16 v28; // r12
    int v29; // r8
    unsigned __int16 ll; // r11
    int v31; // r30
    unsigned __int16 v32; // r8
    unsigned __int8 *v33; // r8
    unsigned __int16 mm; // r12
    __int16 nn; // r7
    unsigned __int16 i1; // r11
    unsigned __int8 v37; // r31
    unsigned __int16 v38; // r12
    int v39; // r8
    unsigned __int16 i2; // r11
    unsigned __int8 *v41; // r8
    unsigned __int16 i3; // r12
    __int16 i4; // r7
    unsigned __int16 i5; // r11
    unsigned __int8 v45; // r26
    unsigned __int8 *v46; // r8
    unsigned __int16 i6; // r12
    unsigned __int16 i7; // r11
    unsigned __int8 v49; // r7

    v6 = buffer;
    v7 = 0;
    v8 = (unsigned __int16)bufwidth;
    v9 = 4 * (unsigned __int16)bufwidth;
    v10 = (unsigned __int16)(blocksperrow * numrows);
    while ((unsigned __int16)v7 < v10) {
        v11 = dont_byteswap_ushort(*(_WORD *)pointers);
        if ((v11 & 0x8000) != 0) {
            v12 = v11 & 0xF000;
            if (v12 == 0xC000) {
                v18 = v11 & 0xFFF;
                pointers += 2;
                for (i = 0; i < v18 >> 1; ++i) {
                    v20 = dont_byteswap_ushort(*(_WORD *)pointers);
                    if ((unsigned __int16)(v20 & 0xFF00) >> 8) {
                        for (j = 0; j < 4u; ++j) {
                            for (k = 0; k < 4u; ++k) {
                                *buffer++ = (unsigned __int16)(v20 & 0xFF00) >> 8;
                            }
                            buffer = &buffer[v8 - 4];
                        }
                        buffer -= v9;
                    }
                    v23 = buffer + 4;
                    if ((_BYTE)v20) {
                        for (l = 0; l < 4u; ++l) {
                            for (m = 0; m < 4u; ++m) {
                                *v23++ = v20;
                            }
                            v23 = &v23[v8 - 4];
                        }
                        v23 -= v9;
                    }
                    buffer = v23 + 4;
                    pointers += 2;
                }
                v7 += v18;
            } else if ((v11 & 0xF000u) >= 0xC000) {
                if (v12 == 0xF000) {
                    if ((_BYTE)v11) {
                        for (n = 0; n < 4u; ++n) {
                            for (ii = 0; ii < 4u; ++ii) {
                                *buffer++ = v11;
                            }
                            buffer = &buffer[v8 - 4];
                        }
                        buffer -= v9;
                    }
                    buffer += 4;
                    pointers += 2;
                    ++v7;
                } else if (v12 == 0xE000) {
                    v15 = &codebook[(16 * v11) & 0xFFF0];
                    for (jj = 0; jj < 4u; ++jj) {
                        for (kk = 0; kk < 4u; ++kk) {
                            if (*v15) {
                                *buffer = *v15;
                            }
                            ++buffer;
                            ++v15;
                        }
                        buffer = &buffer[v8 - 4];
                    }
                    buffer = &buffer[-v9 + 4];
                    pointers += 2;
                    ++v7;
                }
            } else if (v12 == 0xB000) {
                v26 = v11;
                v27 = HIBYTE(v11) & 0xF;
                if ((_BYTE)v11) {
                    v28 = 0;
                    v29 = (4 * v27) & 0x3FFFC;
                    while (v28 < 4u) {
                        for (ll = 0; ll < v29; ++ll) {
                            *buffer++ = v26;
                        }
                        buffer = &buffer[v8 - v29];
                        ++v28;
                    }
                    buffer -= v9;
                }
                buffer += (4 * v27) & 0x3FFFC;
                v7 += v27;
                pointers += 2;
            } else if ((v11 & 0xF000u) < 0xB000 && v12 == 0xA000) {
                pointers += 2;
                v31 = v11 & 0xFFF;
                v32 = dont_byteswap_ushort(*(_WORD *)pointers);
                if ((v32 & 0x8000) != 0) {
                    if ((v32 & 0xC000) == 49152) {
                        v33 = &codebook[(16 * v32) & 0xFFF0];
                        for (mm = 0; mm < 4u; ++mm) {
                            for (nn = 0; nn < (int)(unsigned __int16)v31; ++nn) {
                                for (i1 = 0; i1 < 4u; ++i1) {
                                    if (*v33) {
                                        *buffer = *v33;
                                    }
                                    ++buffer;
                                    ++v33;
                                }
                                v33 -= 4;
                            }
                            v33 += 4;
                            buffer = &buffer[v8 + -4 * (unsigned __int16)v31];
                        }
                        buffer = &buffer[((4 * v31) & 0x3FFFC) - v9];
                        pointers += 2;
                        v7 += v31;
                    } else if ((v32 & 0xC000) == 0x8000) {
                        v37 = v32;
                        if ((_BYTE)v32) {
                            v38 = 0;
                            v39 = (4 * v31) & 0x3FFFC;
                            while (v38 < 4u) {
                                for (i2 = 0; i2 < v39; ++i2) {
                                    *buffer++ = v37;
                                }
                                buffer = &buffer[v8 - v39];
                                ++v38;
                            }
                            buffer -= v9;
                        }
                        buffer += (4 * v31) & 0x3FFFC;
                        v7 += v31;
                        pointers += 2;
                    }
                } else {
                    v41 = &codebook[v32];
                    for (i3 = 0; i3 < 4u; ++i3) {
                        for (i4 = 0; i4 < (int)(unsigned __int16)v31; ++i4) {
                            for (i5 = 0; i5 < 4u; ++i5) {
                                v45 = *v41++;
                                *buffer++ = v45;
                            }
                            v41 -= 4;
                        }
                        v41 += 4;
                        buffer = &buffer[v8 + -4 * (unsigned __int16)v31];
                    }
                    buffer = &buffer[((4 * v31) & 0x3FFFC) - v9];
                    pointers += 2;
                    v7 += v31;
                }
            }
        } else {
            v46 = &codebook[v11];
            for (i6 = 0; i6 < 4u; ++i6) {
                for (i7 = 0; i7 < 4u; ++i7) {
                    v49 = *v46++;
                    *buffer++ = v49;
                }
                buffer = &buffer[v8 - 4];
            }
            buffer = &buffer[-v9 + 4];
            pointers += 2;
            ++v7;
        }
        if ((unsigned __int16)v7 == (unsigned __int16)v7 / blocksperrow * blocksperrow) {
            v6 += v9;
            buffer = v6;
        }
    }
}

void UnVQ_4x41(
    uint8_t *codebook, uint8_t *pointers, uint8_t *buffer, unsigned blocks_per_row, unsigned num_rows, unsigned buff_width)
{
    uint32_t *cb_offset = (uint32_t *)codebook;
    uint32_t blocks_per_row_left = 0;
    uint8_t block_id[4];
    uint32_t data_end;
    uint32_t vptr_pos;
    uint32_t video_pos;
    uint32_t row_offset;

    video_pos = 0;
    vptr_pos = 0;
    row_offset = 0;
    blocks_per_row_left = blocks_per_row;
    data_end = blocks_per_row * num_rows;

    while (vptr_pos < data_end) {
        block_id[0] = pointers[vptr_pos];
        block_id[1] = pointers[vptr_pos + blocks_per_row * num_rows];
        block_id[2] = 0;
        block_id[3] = 0;
        vptr_pos++;

        if (block_id[1] == 255) {
            // make all bytes same color as first byte
            block_id[1] = block_id[0];
            block_id[2] = block_id[0];
            block_id[3] = block_id[0];
            *(uint32_t *)&buffer[video_pos] = *(uint32_t *)block_id;
            *(uint32_t *)&buffer[video_pos + 1 * buff_width] = *(uint32_t *)block_id;
            *(uint32_t *)&buffer[video_pos + 2 * buff_width] = *(uint32_t *)block_id;
            *(uint32_t *)&buffer[video_pos + 3 * buff_width] = *(uint32_t *)block_id;
        } else {
            *(uint32_t *)&buffer[video_pos] = cb_offset[4 * *(uint32_t *)block_id];
            *(uint32_t *)&buffer[video_pos + 1 * buff_width] = cb_offset[4 * *(uint32_t *)block_id + 1];
            *(uint32_t *)&buffer[video_pos + 2 * buff_width] = cb_offset[4 * *(uint32_t *)block_id + 2];
            *(uint32_t *)&buffer[video_pos + 3 * buff_width] = cb_offset[4 * *(uint32_t *)block_id + 3];
        }

        video_pos += 4;
        blocks_per_row_left--;

        if (blocks_per_row_left == 0) {
            row_offset += 4 * buff_width; // move pointer in video memory by four rows
            video_pos = row_offset;
            blocks_per_row_left = blocks_per_row;
        }
    }
}
void unvq_0(unsigned __int8 *codebook, unsigned __int8 *pointers, unsigned __int8 *buffer, unsigned int blocksperrow,
    unsigned int numrows, unsigned int bufwidth)
{
    unsigned __int8 *v8; // r5
    unsigned int v9; // r9
    unsigned int v10; // r0
    unsigned __int8 *v11; // r8
    unsigned int v12; // r0
    int v13; // r12
    int v14; // r31
    unsigned int i; // ctr
    int v16; // r29
    int v17; // r11
    int v18; // r29
    int v19; // r11
    unsigned __int8 v20; // r30
    unsigned __int8 v21; // r12
    int v22; // r29
    unsigned __int8 *v23; // r11
    unsigned __int8 v24; // r28
    unsigned __int8 *v25; // r30
    unsigned __int8 v26; // r27
    int v27; // r29
    int v28; // r12
    int v29; // r12
    int v30; // r12
    int v31; // r31
    int v32; // r31
    int v33; // r31

    unsigned char *data = buffer;
    int ydelta = 0;
    v8 = &buffer[(200 - (__int16)ydelta) * bufwidth];
    v9 = blocksperrow * numrows;
    v10 = 4 * bufwidth;
    v11 = &v8[2 * bufwidth];
    v12 = v10 - 8 * blocksperrow;
    while (numrows) {
        v13 = 0;
        v14 = 0;
        for (i = blocksperrow; i; --i) {
            v16 = pointers[v9];
            v17 = *pointers++;
            v18 = (v16 << 8) + v17;
            if ((v18 & 0xF00) == 0xF00) {
                v19 = v14 << 8;
                v14 = (unsigned __int8)v18;
                v20 = data[(v13 << 8) | (unsigned __int8)v18];
                v21 = data[v19 | (unsigned __int8)v18];
                v22 = ~(((((unsigned __int8)v18 << 8) | (unsigned __int8)v18) << 16) | ((unsigned __int8)v18 << 8)
                    | (unsigned __int8)v18);
                *(_DWORD *)v8 = v22;
                v23 = v8;
                v24 = ~v20;
                *((_DWORD *)v8 + 1) = v22;
                v25 = v11;
                v26 = ~v21;
                *(_DWORD *)v11 = v22;
                v13 = v14;
                v8 += 8;
                *((_DWORD *)v11 + 1) = v22;
                v11 += 8;
                *v23 = v24;
                *v25 = v26;
            } else {
                v27 = 8 * v18;
                *v8 = ~data[(v13 << 8) | codebook[v27]];
                v28 = codebook[v27];
                v8[1] = ~(_BYTE)v28;
                v8[2] = ~data[(v28 << 8) | codebook[v27 + 1]];
                v29 = codebook[v27 + 1];
                v8[3] = ~(_BYTE)v29;
                v8[4] = ~data[(v29 << 8) | codebook[v27 + 2]];
                v30 = codebook[v27 + 2];
                v8[5] = ~(_BYTE)v30;
                v8[6] = ~data[(v30 << 8) | codebook[v27 + 3]];
                v13 = codebook[v27 + 3];
                v8[7] = ~(_BYTE)v13;
                v8 += 8;
                *v11 = ~data[(v14 << 8) | codebook[v27 + 4]];
                v31 = codebook[v27 + 4];
                v11[1] = ~(_BYTE)v31;
                v11[2] = ~data[(v31 << 8) | codebook[v27 + 5]];
                v32 = codebook[v27 + 5];
                v11[3] = ~(_BYTE)v32;
                v11[4] = ~data[(v32 << 8) | codebook[v27 + 6]];
                v33 = codebook[v27 + 6];
                v11[5] = ~(_BYTE)v33;
                v11[6] = ~data[(v33 << 8) | codebook[v27 + 7]];
                v14 = codebook[v27 + 7];
                v11[7] = ~(_BYTE)v14;
                v11 += 8;
            }
        }
        v8 += v12;
        v11 += v12;
        --numrows;
    }
}
unsigned __int8 vqa_codebook[256] = {
    255u,
    1u,
    2u,
    3u,
    4u,
    5u,
    6u,
    7u,
    8u,
    9u,
    10u,
    11u,
    12u,
    13u,
    14u,
    15u,
    16u,
    17u,
    18u,
    19u,
    20u,
    21u,
    22u,
    23u,
    24u,
    25u,
    26u,
    27u,
    28u,
    29u,
    30u,
    31u,
    32u,
    33u,
    34u,
    35u,
    36u,
    37u,
    38u,
    39u,
    40u,
    41u,
    42u,
    43u,
    44u,
    45u,
    46u,
    47u,
    48u,
    49u,
    50u,
    51u,
    52u,
    53u,
    54u,
    55u,
    56u,
    57u,
    58u,
    59u,
    60u,
    61u,
    62u,
    63u,
    64u,
    65u,
    66u,
    67u,
    68u,
    69u,
    70u,
    71u,
    72u,
    73u,
    74u,
    75u,
    76u,
    77u,
    78u,
    79u,
    80u,
    81u,
    82u,
    83u,
    84u,
    85u,
    86u,
    87u,
    88u,
    89u,
    90u,
    91u,
    92u,
    93u,
    94u,
    95u,
    96u,
    97u,
    98u,
    99u,
    100u,
    101u,
    102u,
    103u,
    104u,
    105u,
    106u,
    107u,
    108u,
    109u,
    110u,
    111u,
    112u,
    113u,
    114u,
    115u,
    116u,
    117u,
    118u,
    119u,
    120u,
    121u,
    122u,
    123u,
    124u,
    125u,
    126u,
    127u,
    128u,
    129u,
    130u,
    131u,
    132u,
    133u,
    134u,
    135u,
    136u,
    137u,
    138u,
    139u,
    140u,
    141u,
    142u,
    143u,
    144u,
    145u,
    146u,
    147u,
    148u,
    149u,
    150u,
    151u,
    152u,
    153u,
    154u,
    155u,
    156u,
    157u,
    158u,
    159u,
    160u,
    161u,
    162u,
    163u,
    164u,
    165u,
    166u,
    167u,
    168u,
    169u,
    170u,
    171u,
    172u,
    173u,
    174u,
    175u,
    176u,
    177u,
    178u,
    179u,
    180u,
    181u,
    182u,
    183u,
    184u,
    185u,
    186u,
    187u,
    188u,
    189u,
    190u,
    191u,
    192u,
    193u,
    194u,
    195u,
    196u,
    197u,
    198u,
    199u,
    200u,
    201u,
    202u,
    203u,
    204u,
    205u,
    206u,
    207u,
    208u,
    209u,
    210u,
    211u,
    212u,
    213u,
    214u,
    215u,
    216u,
    217u,
    218u,
    219u,
    220u,
    221u,
    222u,
    223u,
    224u,
    225u,
    226u,
    227u,
    228u,
    229u,
    230u,
    231u,
    232u,
    233u,
    234u,
    235u,
    236u,
    237u,
    238u,
    239u,
    240u,
    241u,
    242u,
    243u,
    244u,
    245u,
    246u,
    247u,
    248u,
    249u,
    250u,
    251u,
    252u,
    253u,
    254u,
    0u,
};


void __cdecl UnVQ_4x422(unsigned __int8 *codebook, unsigned __int8 *pointers, unsigned __int8 *buffer,
    unsigned int blocks_per_row, unsigned int num_rows, unsigned int bufwidth)
{
    return;
    unsigned __int8 *v6; // r10
    int v7; // r11
    int v8; // r27
    int v9; // r0
    unsigned __int16 v10; // r9
    int v11; // r12
    unsigned __int16 v12; // r31
    int v13; // r30
    int v14; // r30
    unsigned __int8 *v15; // r9
    unsigned __int16 j; // r31
    unsigned __int16 k; // r12
    int v18; // r8
    int v19; // r29
    int i; // r8
    unsigned __int16 v21; // r9
    unsigned __int16 v22; // r31
    int v23; // r30
    int v24; // r30
    unsigned __int8 *v25; // r5
    unsigned __int16 v26; // r31
    int v27; // r30
    int v28; // r30
    unsigned __int16 v29; // r31
    int v30; // r30
    int v31; // r30
    unsigned __int16 l; // r12
    int v33; // r29
    unsigned __int16 v34; // r9
    unsigned __int8 *v35; // r9
    unsigned __int16 m; // r31
    int n; // r8
    unsigned __int16 ii; // r12
    int v39; // r25
    unsigned __int16 v40; // r31
    int v41; // r30
    int v42; // r30
    unsigned __int16 jj; // r12
    unsigned __int8 *v44; // r9
    unsigned __int16 kk; // r31
    int v46; // r8
    int v47; // r30
    int v48; // r12
    unsigned __int8 *v49; // r9
    unsigned __int16 v50; // r31

    v6 = buffer;
    v7 = 0;
    v8 = (unsigned __int16)bufwidth;
    v9 = 4 * (unsigned __int16)bufwidth;
    while ((unsigned __int16)v7 < (unsigned int)(unsigned __int16)(blocks_per_row * num_rows)) {
        v10 = dont_byteswap_ushort(*(_WORD *)pointers);
        if ((v10 & 0x8000) != 0) {
            v11 = v10 & 0xF000;
            if (v11 == 49152) {
                v19 = v10 & 0xFFF;
                pointers += 2;
                for (i = 0; i < v19 >> 1; ++i) {
                    v21 = dont_byteswap_ushort(*(_WORD *)pointers);
                    if ((unsigned __int16)(v21 & 0xFF00) >> 8) {
                        v22 = 0;
                        v23 = ((unsigned __int8)vqa_codebook[(unsigned __int16)(v21 & 0xFF00) >> 8] << 8)
                            | (unsigned __int8)vqa_codebook[(unsigned __int16)(v21 & 0xFF00) >> 8];
                        v24 = (v23 << 16) | v23;
                        while (v22 < 4u) {
                            *(_DWORD *)buffer = v24;
                            buffer += v8;
                            ++v22;
                        }
                        buffer -= v9;
                    }
                    v25 = buffer + 4;
                    if ((_BYTE)v21) {
                        v26 = 0;
                        v27 = ((unsigned __int8)vqa_codebook[(unsigned __int8)v21] << 8)
                            | (unsigned __int8)vqa_codebook[(unsigned __int8)v21];
                        v28 = (v27 << 16) | v27;
                        while (v26 < 4u) {
                            *(_DWORD *)v25 = v28;
                            v25 += v8;
                            ++v26;
                        }
                        v25 -= v9;
                    }
                    buffer = v25 + 4;
                    pointers += 2;
                }
                v7 += v19;
            } else if ((v10 & 0xF000u) >= 0xC000) {
                if (v11 == 61440) {
                    if ((_BYTE)v10) {
                        v12 = 0;
                        v13 = ((unsigned __int8)vqa_codebook[(unsigned __int8)v10] << 8)
                            | (unsigned __int8)vqa_codebook[(unsigned __int8)v10];
                        v14 = (v13 << 16) | v13;
                        while (v12 < 4u) {
                            *(_DWORD *)buffer = v14;
                            buffer += v8;
                            ++v12;
                        }
                        buffer -= v9;
                    }
                    buffer += 4;
                    pointers += 2;
                    ++v7;
                } else if (v11 == 57344) {
                    v15 = &codebook[16 * (v10 & 0xFFF)];
                    for (j = 0; j < 4u; ++j) {
                        for (k = 0; k < 4u; ++k) {
                            v18 = v15[k];
                            if (v18 != 255) {
                                buffer[k] = v18;
                            }
                        }
                        v15 += 4;
                        buffer += v8;
                    }
                    buffer = &buffer[-v9 + 4];
                    pointers += 2;
                    ++v7;
                }
            } else if (v11 == 45056) {
                if ((_BYTE)v10) {
                    v29 = 0;
                    v30 = ((unsigned __int8)vqa_codebook[(unsigned __int8)v10] << 8)
                        | (unsigned __int8)vqa_codebook[(unsigned __int8)v10];
                    v31 = (v30 << 16) | v30;
                    while (v29 < 4u) {
                        for (l = 0; l < (HIBYTE(v10) & 0xFu); ++l) {
                            *(_DWORD *)&buffer[4 * l] = v31;
                        }
                        buffer += v8;
                        ++v29;
                    }
                    buffer -= v9;
                }
                buffer += 4 * (HIBYTE(v10) & 0xF);
                v7 += HIBYTE(v10) & 0xF;
                pointers += 2;
            } else if ((v10 & 0xF000u) < 0xB000 && v11 == 40960) {
                v33 = v10 & 0xFFF;
                pointers += 2;
                v34 = dont_byteswap_ushort(*(_WORD *)pointers);
                if ((v34 & 0x8000) != 0) {
                    if ((v34 & 0xC000) == 49152) {
                        v35 = &codebook[16 * (v34 & 0xFFF)];
                        for (m = 0; m < 4u; ++m) {
                            for (n = 0; n < (unsigned __int16)v33; ++n) {
                                for (ii = 0; ii < 4u; ++ii) {
                                    v39 = v35[ii];
                                    if (v39 != 255) {
                                        buffer[ii] = v39;
                                    }
                                }
                                buffer += 4;
                            }
                            v35 += 4;
                            buffer = &buffer[v8 + -4 * (unsigned __int16)v33];
                        }
                        buffer = &buffer[4 * (unsigned __int16)v33 - v9];
                        pointers += 2;
                        v7 += v33;
                    } else if ((v34 & 0xC000) == 0x8000) {
                        if ((_BYTE)v34) {
                            v40 = 0;
                            v41 = ((unsigned __int8)vqa_codebook[(unsigned __int8)v34] << 8)
                                | (unsigned __int8)vqa_codebook[(unsigned __int8)v34];
                            v42 = (v41 << 16) | v41;
                            while (v40 < 4u) {
                                for (jj = 0; jj < (unsigned int)(unsigned __int16)v33; ++jj) {
                                    *(_DWORD *)&buffer[4 * jj] = v42;
                                }
                                buffer += v8;
                                ++v40;
                            }
                            buffer -= v9;
                        }
                        buffer += 4 * (unsigned __int16)v33;
                        v7 += v33;
                        pointers += 2;
                    }
                } else {
                    v44 = &codebook[v34];
                    for (kk = 0; kk < 4u; ++kk) {
                        v46 = 0;
                        v47 = *(_DWORD *)v44;
                        v48 = 0;
                        while (v46 < (unsigned __int16)v33) {
                            *(_DWORD *)&buffer[v48] = v47;
                            ++v46;
                            v48 += 4;
                        }
                        v44 += 4;
                        buffer += v8;
                    }
                    buffer = &buffer[4 * (unsigned __int16)v33 - v9];
                    pointers += 2;
                    v7 += v33;
                }
            }
        } else {
            v49 = &codebook[v10];
            v50 = 0;
            while (v50 < 4u) {
                ++v50;
                *(_DWORD *)buffer = *(_DWORD *)v49;
                v49 += 4;
                buffer += v8;
            }
            buffer = &buffer[-v9 + 4];
            pointers += 2;
            ++v7;
        }
        if ((unsigned __int16)v7 == (unsigned __int16)v7 / blocks_per_row * blocks_per_row) {
            v6 += v9;
            buffer = v6;
        }
    }
}

void __cdecl UnVQ4x4333(unsigned __int8 *codebook, unsigned __int8 *pointers, unsigned __int8 *buffer,
    unsigned int blocksperrow, unsigned int numrows, unsigned int bufwidth)
{
    unsigned __int8 *v6; // r9
    int v7; // r10
    int v8; // r28
    int v9; // r0
    unsigned int v10; // r29
    unsigned __int16 v11; // r8
    int v12; // r11
    unsigned __int16 n; // r12
    unsigned __int16 ii; // r11
    unsigned __int8 *v15; // r8
    unsigned __int16 jj; // r12
    unsigned __int16 kk; // r11
    int v18; // r30
    __int16 i; // r7
    unsigned __int16 v20; // r8
    unsigned __int16 j; // r12
    unsigned __int16 k; // r11
    unsigned __int8 *v23; // r5
    unsigned __int16 l; // r12
    unsigned __int16 m; // r11
    unsigned __int8 v26; // r31
    int v27; // r30
    unsigned __int16 v28; // r12
    int v29; // r8
    unsigned __int16 ll; // r11
    int v31; // r30
    unsigned __int16 v32; // r8
    unsigned __int8 *v33; // r8
    unsigned __int16 mm; // r12
    __int16 nn; // r7
    unsigned __int16 i1; // r11
    unsigned __int8 v37; // r31
    unsigned __int16 v38; // r12
    int v39; // r8
    unsigned __int16 i2; // r11
    unsigned __int8 *v41; // r8
    unsigned __int16 i3; // r12
    __int16 i4; // r7
    unsigned __int16 i5; // r11
    unsigned __int8 v45; // r26
    unsigned __int8 *v46; // r8
    unsigned __int16 i6; // r12
    unsigned __int16 i7; // r11
    unsigned __int8 v49; // r7

    v6 = buffer;
    v7 = 0;
    v8 = (unsigned __int16)bufwidth;
    v9 = 4 * (unsigned __int16)bufwidth;
    v10 = (unsigned __int16)(blocksperrow * numrows);
    while ((unsigned __int16)v7 < v10) {
        v11 = dont_byteswap_ushort(*(_WORD *)pointers);
        if ((v11 & 0x8000) != 0) {
            v12 = v11 & 0xF000;
            if (v12 == 0xC000) {
                v18 = v11 & 0xFFF;
                pointers += 2;
                for (i = 0; i < v18 >> 1; ++i) {
                    v20 = dont_byteswap_ushort(*(_WORD *)pointers);
                    if ((unsigned __int16)(v20 & 0xFF00) >> 8) {
                        for (j = 0; j < 4u; ++j) {
                            for (k = 0; k < 4u; ++k) {
                                *buffer++ = (unsigned __int16)(v20 & 0xFF00) >> 8;
                            }
                            buffer = &buffer[v8 - 4];
                        }
                        buffer -= v9;
                    }
                    v23 = buffer + 4;
                    if ((_BYTE)v20) {
                        for (l = 0; l < 4u; ++l) {
                            for (m = 0; m < 4u; ++m) {
                                *v23++ = v20;
                            }
                            v23 = &v23[v8 - 4];
                        }
                        v23 -= v9;
                    }
                    buffer = v23 + 4;
                    pointers += 2;
                }
                v7 += v18;
            } else if ((v11 & 0xF000u) >= 0xC000) {
                if (v12 == 0xF000) {
                    if ((_BYTE)v11) {
                        for (n = 0; n < 4u; ++n) {
                            for (ii = 0; ii < 4u; ++ii) {
                                *buffer++ = v11;
                            }
                            buffer = &buffer[v8 - 4];
                        }
                        buffer -= v9;
                    }
                    buffer += 4;
                    pointers += 2;
                    ++v7;
                } else if (v12 == 0xE000) {
                    v15 = &codebook[(16 * v11) & 0xFFF0];
                    for (jj = 0; jj < 4u; ++jj) {
                        for (kk = 0; kk < 4u; ++kk) {
                            if (*v15) {
                                *buffer = *v15;
                            }
                            ++buffer;
                            ++v15;
                        }
                        buffer = &buffer[v8 - 4];
                    }
                    buffer = &buffer[-v9 + 4];
                    pointers += 2;
                    ++v7;
                }
            } else if (v12 == 0xB000) {
                v26 = v11;
                v27 = HIBYTE(v11) & 0xF;
                if ((_BYTE)v11) {
                    v28 = 0;
                    v29 = (4 * v27) & 0x3FFFC;
                    while (v28 < 4u) {
                        for (ll = 0; ll < v29; ++ll) {
                            *buffer++ = v26;
                        }
                        buffer = &buffer[v8 - v29];
                        ++v28;
                    }
                    buffer -= v9;
                }
                buffer += (4 * v27) & 0x3FFFC;
                v7 += v27;
                pointers += 2;
            } else if ((v11 & 0xF000u) < 0xB000 && v12 == 0xA000) {
                pointers += 2;
                v31 = v11 & 0xFFF;
                v32 = dont_byteswap_ushort(*(_WORD *)pointers);
                if ((v32 & 0x8000) != 0) {
                    if ((v32 & 0xC000) == 49152) {
                        v33 = &codebook[(16 * v32) & 0xFFF0];
                        for (mm = 0; mm < 4u; ++mm) {
                            for (nn = 0; nn < (int)(unsigned __int16)v31; ++nn) {
                                for (i1 = 0; i1 < 4u; ++i1) {
                                    if (*v33) {
                                        *buffer = *v33;
                                    }
                                    ++buffer;
                                    ++v33;
                                }
                                v33 -= 4;
                            }
                            v33 += 4;
                            buffer = &buffer[v8 + -4 * (unsigned __int16)v31];
                        }
                        buffer = &buffer[((4 * v31) & 0x3FFFC) - v9];
                        pointers += 2;
                        v7 += v31;
                    } else if ((v32 & 0xC000) == 0x8000) {
                        v37 = v32;
                        if ((_BYTE)v32) {
                            v38 = 0;
                            v39 = (4 * v31) & 0x3FFFC;
                            while (v38 < 4u) {
                                for (i2 = 0; i2 < v39; ++i2) {
                                    *buffer++ = v37;
                                }
                                buffer = &buffer[v8 - v39];
                                ++v38;
                            }
                            buffer -= v9;
                        }
                        buffer += (4 * v31) & 0x3FFFC;
                        v7 += v31;
                        pointers += 2;
                    }
                } else {
                    v41 = &codebook[v32];
                    for (i3 = 0; i3 < 4u; ++i3) {
                        for (i4 = 0; i4 < (int)(unsigned __int16)v31; ++i4) {
                            for (i5 = 0; i5 < 4u; ++i5) {
                                v45 = *v41++;
                                *buffer++ = v45;
                            }
                            v41 -= 4;
                        }
                        v41 += 4;
                        buffer = &buffer[v8 + -4 * (unsigned __int16)v31];
                    }
                    buffer = &buffer[((4 * v31) & 0x3FFFC) - v9];
                    pointers += 2;
                    v7 += v31;
                }
            }
        } else {
            v46 = &codebook[v11];
            for (i6 = 0; i6 < 4u; ++i6) {
                for (i7 = 0; i7 < 4u; ++i7) {
                    v49 = *v46++;
                    *buffer++ = v49;
                }
                buffer = &buffer[v8 - 4];
            }
            buffer = &buffer[-v9 + 4];
            pointers += 2;
            ++v7;
        }
        if ((unsigned __int16)v7 == (unsigned __int16)v7 / blocksperrow * blocksperrow) {
            v6 += v9;
            buffer = v6;
        }
    }
}

void Setup_Hooks()
{
    // Put hooks here, e.g. Hook_Function(0xDEADBEEF, Some_Function)
    // MessageBox(0, "FOOO6y6yOO", "FUUUUK", 0);
    // disable VQA MCGA drawing
    *(int *)(0x00447AE8 + 4) = 0;
    *(int *)(0x00447AE8 + 8) = 0;
    Hook_Function(0x00429DD8, Get_Executable_And_CD_Paths1);
    Hook_Function(0x00434C3C, UnVQ_4x422);
    //Hook_Function(0x00434C3C, UnVQ4x4333);
    //Hook_Function(0x00434F8E, UnVQ_4x422);
}

static const int nBinarySize = 0x0050000; // Size of game binary

// HANDLE hProcess = GetCurrentProcess();
DWORD OldProtect;

void StartHooking()
{
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)0x00401000, nBinarySize, PAGE_EXECUTE_READWRITE, &OldProtect);
}

void StopHooking()
{
    DWORD OldProtect2;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)0x00401000, nBinarySize, OldProtect, &OldProtect2);
}


// Use DLLMain to Set up our hooks when the DLL loads. The launcher should stall
// the main thread at the entry point so hooked code called after that should
// be our code.
BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            StartHooking();
            Setup_Hooks();
            break;

        case DLL_PROCESS_DETACH:
            // StopHooking();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return TRUE;
}
