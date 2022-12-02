#include <cstdlib>
#include "../mem/wwmem.h"
#include "file.h"
#include "filetemp.h"
#include "_file.h"

int __cdecl Do_Open_Error(FileErrorType errormsgnum, char const *file_name)
{
    return 0;
}
void __cdecl Do_IO_Error(FileErrorType errormsgnum, char const *filename)
{
    Prog_End();
    exit(errormsgnum);
}
long __cdecl Read_File_With_Recovery(int handle, void *buf, unsigned int bytes)
{
    int bytes_read;

    while (true) {
        Hard_Error_Occured = 0;
        ibm_chdir(DataPath);
        int newhandle = Open_File(FileHandleTable[handle].Name, FileHandleTable[handle].Mode);
        Seek_File(newhandle, FileHandleTable[handle].Pos, 0);
        ibm_close(FileHandleTable[handle].Handle);
        Mem_Copy(&FileHandleTable[newhandle], &FileHandleTable[handle], sizeof(FileHandleType));
        FileHandleTable[newhandle].Empty = 1;
        bytes_read = ibm_read(FileHandleTable[handle].Handle, buf, bytes);
        if (!Hard_Error_Occured) {
            break;
        }

        if (!Do_Open_Error(COULD_NOT_OPEN, FileHandleTable[handle].Name)) {
            return 0;
        }

        if (!ibm_chdir(DataPath)) {
            return 0;
        }
    }
    return bytes_read;
}

int __cdecl Open_File_With_Recovery(char const *file_name, unsigned int mode)
{
    Hard_Error_Occured = 0;
    int v2 = 256;
    if (mode != 32768) {
        v2 = 384;
    }

    int handle = ibm_open(file_name, mode, v2);
    if (!Hard_Error_Occured || !UseCD || ibm_getdisk() != DataPath[0] - 'A') {
        return handle;
    }
    Hard_Error_Occured = 0;
    ibm_setdisk(DataPath[0] - 'A');
    ibm_chdir(DataPath);

    int v5 = 256;
    if (mode != 32768) {
        v5 = 384;
    }

    handle = ibm_open(file_name, mode, v5);

    if (!Hard_Error_Occured) {
        return handle;
    }

    Hard_Error_Occured = 0;
    handle = -1;
    return handle;
}

int __cdecl Cache_File(int index, int file_handle)
{
    if (index == -1) {
        return 0;
    }

    FileDataType *filedata = &FileDataPtr[index];

    if (!(filedata->Flag & 0x600) || filedata->Ptr) {
        return 0;
    }
    int filesize = filedata->Size;

    if (filesize > Mem_Pool_Size(FileCacheHeap)) {
        filedata->Flag &= 0xFFFFD1FF;
        return 0;
    }

    while (filesize > Mem_Avail(FileCacheHeap)) {
        void *node = Mem_Find_Oldest(FileCacheHeap);
        if (!node) {
            return 0;
        }
        filedata = &FileDataPtr[Mem_Get_ID(node)];
        Mem_Free(FileCacheHeap, filedata->Ptr);
        filedata->Ptr = 0;
    }

    if (filesize > Mem_Largest_Avail(FileCacheHeap)) {
        Unfragment_File_Cache();
    }

    if (filesize >= Mem_Largest_Avail(FileCacheHeap)) {
        return 0;
    }

    FileHandleType *filehandletable = &FileHandleTable[file_handle];
    FileDataType *parent = &FileDataPtr[index];
    parent->Ptr = Mem_Alloc(FileCacheHeap, filesize, index);
    if (!parent->Ptr) {
        return 0;
    }
    Mem_In_Use(parent->Ptr);

    Read_File(file_handle, parent->Ptr, filesize);

    filehandletable->Pos = 0;
    filehandletable->Start = 0;

    if (parent->Flag & 4096) {
        --FileDataPtr[parent->Disk].OpenCount;
    }

    ibm_close(filehandletable->Handle);
    filehandletable->Handle = 0;
    return 1;
}