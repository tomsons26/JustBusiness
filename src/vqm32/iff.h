#pragma once

struct _ChunkHeader
{
    uint32_t id;
    uint32_t size;
};
typedef _ChunkHeader ChunkHeader;