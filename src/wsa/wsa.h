#pragma once
#include <cstdint>

typedef enum : int16_t
{
    WSA_NORMAL,
    WSA_BOTTOM_UP = 0x1000,
    WSA_TRANS = 0x2000,
    WSA_SHADING = 0x4000,
    WSA_PRIORITY = 0x8000,
} WSAType;

typedef enum : int16_t
{
    WSA_OPEN_FROM_MEM = 0x0000,
    WSA_OPEN_INDIRECT = 0x0000,
    WSA_OPEN_FROM_DISK = 0x0001,
    WSA_OPEN_DIRECT = 0x0002,

    WSA_OPEN_TO_PAGE = WSA_OPEN_DIRECT,
    WSA_OPEN_TO_BUFFER = WSA_OPEN_INDIRECT,

} WSAOpenType;