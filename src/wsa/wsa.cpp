#include "wsa.h"
#include "..\file\file.h"
#include "..\mem\wwmem.h"
#include "..\mcgaprim\gbuffer.h"
#include <minwindef.h>
#define TRUE 0x01
#define FALSE 0x00
#ifndef PRIVATE
#define PRIVATE static
#endif

// temp

template<class T>
T ABS(T a)
{
    return (a < 0) ? -a : a;
}
int __cdecl Buffer_To_Page_With_Flags(int x, int y, int w, int h, void *Buffer, GraphicViewPortClass &view,
    unsigned short flags, char *shading_tbl, char shading_col)
{
    return 0;
}
unsigned int __cdecl Apply_XOR_Delta(char *source_ptr, char *delta_ptr)
{
    return 0;
}
void __cdecl Apply_XOR_Delta_To_Page_Or_Viewport(void *target, void *delta, short selector, unsigned short width, short nextrow, short copy)
{
    
}
unsigned int __cdecl LCW_Uncompress(void *source, void *dest, unsigned int length)
{
    return 0;
}

#define WSA_USER_ALLOCATED 0x01
#define WSA_SYS_ALLOCATED 0x02
#define WSA_FILE 0x04
#define WSA_RESIDENT 0x08
#define WSA_TARGET_IN_BUFFER 0x10
#define WSA_LINEAR_ONLY 0x20
#define WSA_FRAME_0_ON_PAGE 0x40
#define WSA_AMIGA_ANIMATION 0x80
#define WSA_PALETTE_PRESENT 0x100
#define WSA_FRAME_0_IS_DELTA 0x200

#define DO_XOR 0x0
#define DO_COPY 0x01



extern void *Add_Long_To_Pointer(void const *ptr, long size);
#pragma pack(push, 1)
typedef struct
{
    unsigned short current_frame;
    unsigned short total_frames;
    unsigned short pixel_x;
    unsigned short pixel_y;
    unsigned short pixel_width;
    unsigned short pixel_height;
    unsigned int largest_frame_size;
    char *delta_buffer;
    char *file_buffer;
    char file_name[13];
    short flags;
    short file_handle;
    unsigned int anim_mem_size;
} SysAnimHeaderType;
#pragma pack(pop)

#define EXTRA_charS_ANIMATE_NOT_KNOW_ABOUT (sizeof(short) + sizeof(unsigned long))

#pragma pack(push, 1)
typedef struct
{
    unsigned short total_frames;
    unsigned short pixel_x;
    unsigned short pixel_y;
    unsigned short pixel_width;
    unsigned short pixel_height;
    unsigned int largest_frame_size;
    short flags;
    unsigned int frame0_offset;
    unsigned int frame0_end;
} WSA_FileHeaderType;

#define WSA_FILE_HEADER_SIZE (sizeof(WSA_FileHeaderType) - (2 * sizeof(unsigned long)))

#pragma pack(pop)

PRIVATE unsigned int Get_Resident_Frame_Offset(char *file_buffer, short frame);
PRIVATE unsigned int Get_File_Frame_Offset(short file_handle, short frame, short palette_adjust);
PRIVATE int Apply_Delta(SysAnimHeaderType *sys_header, short curr_frame, char *dest_ptr, short dest_sel, short dest_w);


void *__cdecl Open_Animation(
    char *file_name, char *user_buffer, long user_buffer_size, WSAOpenType user_flags, unsigned char *palette)
{
    int fh, anim_flags;
    int palette_adjust;
    unsigned int offsets_size;
    unsigned int frame0_size;
    long target_buffer_size, delta_buffer_size, file_buffer_size;
    long max_buffer_size, min_buffer_size;
    char *sys_anim_header_buffer;
    char *target_buffer;
    char *delta_buffer, *delta_back;
    SysAnimHeaderType *sys_header;
    WSA_FileHeaderType file_header;

    anim_flags = 0;
    fh = Open_File(file_name, READ);
    Read_File(fh, (char *)&file_header, sizeof(WSA_FileHeaderType));

    if (file_header.flags & 1) {
        anim_flags |= WSA_PALETTE_PRESENT;
        palette_adjust = 768;

        if (palette != NULL) {
            Seek_File(fh, sizeof(unsigned long) * (file_header.total_frames), SEEK_CUR);
            Read_File(fh, palette, 768L);
        }

    } else {
        palette_adjust = 0;
    }

    if (file_header.flags & 2) {
        anim_flags |= WSA_FRAME_0_IS_DELTA;
    }

    file_buffer_size = Seek_File(fh, 0L, SEEK_END);

    if (file_header.frame0_offset) {
        long tlong;

        tlong = file_header.frame0_end - file_header.frame0_offset;
        frame0_size = (unsigned short)tlong;
    } else {
        anim_flags |= WSA_FRAME_0_ON_PAGE;
        frame0_size = 0;
    }

    file_buffer_size -= palette_adjust + frame0_size + WSA_FILE_HEADER_SIZE;

    if (user_flags & WSA_OPEN_DIRECT) {
        target_buffer_size = 0L;
    } else {
        anim_flags |= WSA_TARGET_IN_BUFFER;
        target_buffer_size = (unsigned long)file_header.pixel_width * file_header.pixel_height;
    }

    delta_buffer_size = (unsigned long)file_header.largest_frame_size + EXTRA_charS_ANIMATE_NOT_KNOW_ABOUT;
    min_buffer_size = target_buffer_size + delta_buffer_size;
    max_buffer_size = min_buffer_size + file_buffer_size;

    if (user_buffer && (user_buffer_size < min_buffer_size)) {
        Close_File(fh);
        return (NULL);
    }

    if (user_buffer == NULL) {
        if (user_flags & WSA_OPEN_FROM_DISK) {
            user_buffer_size = min_buffer_size;
        }
        else if (!user_buffer_size) {
            user_buffer_size = max_buffer_size;
        }
        else if (user_buffer_size < max_buffer_size) {
            user_buffer_size = min_buffer_size;
        }
        else {
            user_buffer_size = max_buffer_size;
        }

        if (user_buffer_size > Ram_Free(MEM_NORMAL)) {

            if (min_buffer_size > Ram_Free(MEM_NORMAL)) {
                Close_File(fh);
                return (NULL);
            }

            user_buffer_size = min_buffer_size;
        }

        user_buffer = (char *)Alloc(user_buffer_size, MEM_CLEAR);

        anim_flags |= WSA_SYS_ALLOCATED;
    } else {
        if ((user_flags & WSA_OPEN_FROM_DISK) || (user_buffer_size < max_buffer_size)) {
            user_buffer_size = min_buffer_size;
        } else {
            user_buffer_size = max_buffer_size;
        }
        anim_flags |= WSA_USER_ALLOCATED;
    }

    sys_anim_header_buffer = user_buffer;
    target_buffer = (char *)Add_Long_To_Pointer(sys_anim_header_buffer, sizeof(SysAnimHeaderType));
    delta_buffer = (char *)Add_Long_To_Pointer(target_buffer, target_buffer_size);

    if (target_buffer_size) {
        memset(target_buffer, 0, (unsigned short)target_buffer_size);
    }

    sys_header = (SysAnimHeaderType *)sys_anim_header_buffer;
    sys_header->current_frame = sys_header->total_frames = file_header.total_frames;
    sys_header->pixel_x = file_header.pixel_x;
    sys_header->pixel_y = file_header.pixel_y;
    sys_header->pixel_width = file_header.pixel_width;
    sys_header->pixel_height = file_header.pixel_height;
    sys_header->anim_mem_size = user_buffer_size;
    sys_header->delta_buffer = delta_buffer;
    sys_header->largest_frame_size = (unsigned short)(delta_buffer_size - sizeof(SysAnimHeaderType));

    strcpy(sys_header->file_name, file_name);

    offsets_size = (file_header.total_frames + 2) << 2;

    if (user_buffer_size == max_buffer_size) {

        sys_header->file_buffer = (char *)Add_Long_To_Pointer(delta_buffer, sys_header->largest_frame_size);
        Seek_File(fh, WSA_FILE_HEADER_SIZE, SEEK_SET);
        Read_File(fh, sys_header->file_buffer, offsets_size);
        Seek_File(fh, frame0_size + palette_adjust, SEEK_CUR);
        Read_File(fh, sys_header->file_buffer + offsets_size, file_buffer_size - offsets_size);

        if (Get_Resident_Frame_Offset(sys_header->file_buffer, sys_header->total_frames + 1)){
            anim_flags |= WSA_RESIDENT;
        }else{
            anim_flags |= WSA_LINEAR_ONLY | WSA_RESIDENT;}
    } else {

        if (Get_File_Frame_Offset(fh, sys_header->total_frames + 1, palette_adjust)){
            anim_flags |= WSA_FILE;
        }else{
            anim_flags |= WSA_LINEAR_ONLY | WSA_FILE;
        }

        sys_header->file_buffer = NULL;
    }

    delta_back = (char *)Add_Long_To_Pointer(delta_buffer, sys_header->largest_frame_size - frame0_size) - 1;

    Seek_File(fh, WSA_FILE_HEADER_SIZE + offsets_size + palette_adjust, SEEK_SET);
    Read_File(fh, delta_back, frame0_size);
    *(char *)Add_Long_To_Pointer(delta_back, frame0_size) = 128;

    if (anim_flags & WSA_RESIDENT) {
        sys_header->file_handle = (short)-1;
        Close_File(fh);
    } else {
        sys_header->file_handle = (short)fh;
    }

    LCW_Uncompress(delta_back, delta_buffer, sys_header->largest_frame_size);

    sys_header->flags = (short)anim_flags;

    return (user_buffer);
}

void __cdecl Close_Animation(void *handle)
{
    SysAnimHeaderType *sys_header;

    sys_header = (SysAnimHeaderType *)handle;

    if (sys_header->flags & WSA_FILE) {
        Close_File(sys_header->file_handle);
    }

    if (handle && sys_header->flags & WSA_SYS_ALLOCATED) {
        Free(handle);
    }
}

BOOL __cdecl Animate_Frame(void *handle, GraphicViewPortClass &view, short frame_number, short x_pixel, short y_pixel,
    WSAType flags_and_prio, void *shading_tbl, char shading_color)
{
    SysAnimHeaderType *sys_header;
    short curr_frame;
    short total_frames;
    short distance;
    short search_dir;
    short search_frames;
    int loop;
    char *frame_buffer;
    int direct_to_dest;
    int dest_width;
    short selector;

    sys_header = (SysAnimHeaderType *)handle;

    total_frames = sys_header->total_frames;

    if (!handle || (total_frames <= frame_number)) {
        return FALSE;
    }

    selector = view.Get_Selector();
    dest_width = view.Get_Width() + view.Get_XAdd();

    x_pixel += (short)sys_header->pixel_x;
    y_pixel += (short)sys_header->pixel_y;

    if (sys_header->flags & WSA_TARGET_IN_BUFFER) {
        frame_buffer = (char *)Add_Long_To_Pointer(sys_header, sizeof(SysAnimHeaderType));
        direct_to_dest = FALSE;
    } else {
        frame_buffer = (char *)view.Get_Offset();
        frame_buffer += (y_pixel * dest_width) + x_pixel;
        direct_to_dest = TRUE;
    }

    if (sys_header->current_frame == total_frames) {

        if (!(sys_header->flags & WSA_FRAME_0_ON_PAGE)) {
            if (direct_to_dest) {
                Apply_XOR_Delta_To_Page_Or_Viewport(frame_buffer,
                    sys_header->delta_buffer,
                    selector,
                    sys_header->pixel_width,
                    dest_width,
                    (sys_header->flags & WSA_FRAME_0_IS_DELTA) ? DO_XOR : DO_COPY);
            } else {
                Apply_XOR_Delta(frame_buffer, sys_header->delta_buffer);
            }
        }
        sys_header->current_frame = 0;
    }

    curr_frame = sys_header->current_frame;

    distance = ABS(curr_frame - frame_number);

    search_dir = 1;

    if (frame_number > curr_frame) {
        search_frames = total_frames - frame_number + curr_frame;

        if ((search_frames < distance) && !(sys_header->flags & WSA_LINEAR_ONLY)) {
            search_dir = -1;
        } else {
            search_frames = distance;
        }
    } else {
        search_frames = total_frames - curr_frame + frame_number;

        if ((search_frames >= distance) || (sys_header->flags & WSA_LINEAR_ONLY)) {
            search_dir = -1;
            search_frames = distance;
        }
    }

    if (search_dir > 0) {
        for (loop = 0; loop < search_frames; loop++) {
            curr_frame += search_dir;

            Apply_Delta(sys_header, curr_frame, frame_buffer, selector, dest_width);

            if (curr_frame == total_frames) {
                curr_frame = 0;
            }
        }
    } else {
        for (loop = 0; loop < search_frames; loop++) {

            if (curr_frame == 0) {
                curr_frame = total_frames;
            }

            Apply_Delta(sys_header, curr_frame, frame_buffer, selector, dest_width);

            curr_frame += search_dir;
        }
    }

    sys_header->current_frame = frame_number;

    if (sys_header->flags & WSA_TARGET_IN_BUFFER) {
        int flags = ((unsigned short)flags_and_prio & 0xFF00) >> 12u;

        Buffer_To_Page_With_Flags(x_pixel,
            y_pixel,
            sys_header->pixel_width,
            sys_header->pixel_height,
            frame_buffer,
            view,
            flags,
            (char *)shading_tbl,
            shading_color);
    }

    return TRUE;
}

int __cdecl Get_Animation_Frame_Count(void *handle)
{
    SysAnimHeaderType *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return ((short)sys_header->total_frames);
}

int __cdecl Get_Animation_X(void const *handle)
{
    SysAnimHeaderType const *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return (sys_header->pixel_x);
}

int __cdecl Get_Animation_Y(void const *handle)
{
    SysAnimHeaderType const *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return (sys_header->pixel_y);
}

int __cdecl Get_Animation_Width(void const *handle)
{
    SysAnimHeaderType const *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return (sys_header->pixel_width);
}

int __cdecl Get_Animation_Height(void const *handle)
{
    SysAnimHeaderType const *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return (sys_header->pixel_height);
}

int __cdecl Get_Animation_Palette(void const *handle)
{
    SysAnimHeaderType const *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return (sys_header->flags & WSA_PALETTE_PRESENT);
}

unsigned int __cdecl Get_Animation_Size(void const *handle)
{
    SysAnimHeaderType const *sys_header;

    if (!handle) {
        return FALSE;
    }
    sys_header = (SysAnimHeaderType *)handle;
    return (sys_header->anim_mem_size);
}

PRIVATE unsigned int Get_Resident_Frame_Offset(char *file_buffer, short frame)
{
    unsigned int frame0_size;
    unsigned int *lptr;

    lptr = (unsigned int *)file_buffer;

    if (*lptr) {
        frame0_size = lptr[1] - *lptr;
    } else {
        frame0_size = 0;
    }

    lptr += frame;
    if (*lptr)
        return (*lptr - (frame0_size + WSA_FILE_HEADER_SIZE));
    else
        return (0L);
}

PRIVATE unsigned int Get_File_Frame_Offset(short file_handle, short frame, short palette_adjust)
{
    unsigned int offset;

    Seek_File(file_handle, (frame << 2) + WSA_FILE_HEADER_SIZE, SEEK_SET);

    if (Read_File(file_handle, &offset, sizeof(unsigned int)) != sizeof(unsigned int)) {
        offset = 0L;
    }
    offset += palette_adjust;
    return offset;
}

PRIVATE int Apply_Delta(SysAnimHeaderType *sys_header, short curr_frame, char *dest_ptr, short dest_sel, short dest_w)
{
    char *data_ptr;
    char *delta_back;
    short file_handle;
    short palette_adjust;
    unsigned int frame_data_size;
    unsigned int frame_offset;

    palette_adjust = ((sys_header->flags & WSA_PALETTE_PRESENT) ? 768 : 0);
    delta_back = sys_header->delta_buffer;

    if (sys_header->flags & WSA_RESIDENT) {
        frame_offset = Get_Resident_Frame_Offset(sys_header->file_buffer, curr_frame);
        frame_data_size = Get_Resident_Frame_Offset(sys_header->file_buffer, curr_frame + 1) - frame_offset;

        data_ptr = (char *)Add_Long_To_Pointer(sys_header->file_buffer, frame_offset);
        delta_back = (char *)Add_Long_To_Pointer(delta_back, sys_header->largest_frame_size - frame_data_size) - 1;

        Mem_Copy(data_ptr, delta_back, frame_data_size);

    } else if (sys_header->flags & WSA_FILE) {
        file_handle = sys_header->file_handle;
        Seek_File(file_handle, 0, SEEK_SET);

        frame_offset = Get_File_Frame_Offset(file_handle, curr_frame, palette_adjust);
        frame_data_size = Get_File_Frame_Offset(file_handle, curr_frame + 1, palette_adjust) - frame_offset;

        if (!frame_offset || !frame_data_size) {
            return (FALSE);
        }

        Seek_File(file_handle, frame_offset, SEEK_SET);
        delta_back = (char *)Add_Long_To_Pointer(delta_back, sys_header->largest_frame_size - frame_data_size) - 1;

        if (Read_File(file_handle, delta_back, frame_data_size) != frame_data_size) {
            return (FALSE);
        }
    }

    *(char *)Add_Long_To_Pointer(delta_back, frame_data_size) = 128;
    LCW_Uncompress(delta_back, sys_header->delta_buffer, sys_header->largest_frame_size);

    if (sys_header->flags & WSA_TARGET_IN_BUFFER) {
        Apply_XOR_Delta(dest_ptr, sys_header->delta_buffer);
    } else {
        Apply_XOR_Delta_To_Page_Or_Viewport(
            dest_ptr, sys_header->delta_buffer, dest_sel, sys_header->pixel_width, dest_w, DO_XOR);
    }

    return (TRUE);
}
