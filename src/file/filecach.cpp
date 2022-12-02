#include "file.h"
#include "_file.h"
#include "../mem/wwmem.h"

#include <cstring>

void Unfragment_File_Cache()
{
    Mem_Cleanup(FileCacheHeap);
    FileDataType *filedata = &FileDataPtr[NumPAKFiles];
    unsigned int idx = NumPAKFiles;

    while (idx < NumPAKFiles) {
        while (filedata->Name) {
            void *parent = filedata->Ptr;
            if (parent) {
                if (filedata->Flag & 4096) {
                    if (parent == FileDataPtr[filedata->Disk].Ptr) {
                        filedata->Ptr = Mem_Find(FileCacheHeap, filedata->Disk);
                    } else {
                        filedata->Ptr = Mem_Find(FileCacheHeap, idx);
                    }
                }
            } else {
                filedata->Ptr = Mem_Find(FileCacheHeap, idx);
            }
        }
        ++idx;
        ++filedata;
    }

    filedata = FileDataPtr;
    idx = 0;
    while (idx < NumPAKFiles) {
        if (filedata->Ptr) {
            filedata->Ptr = (char *)Mem_Find(FileCacheHeap, idx);
        }
        ++idx;
        ++filedata;
    }
}

int __cdecl Make_File_Resident(char *filename)
{
    int fileindex = Find_File_Index(filename);

    if (fileindex == -1) {
        return 0;
    }

    FileDataType *filedata = &FileDataPtr[fileindex];

    int oldflag = filedata->Flag;

    filedata->Flag |= 1024;
    filedata->Flag &= ~2048;

    int handle = Open_File(filename, 1);
    Close_File(handle);

    filedata->Flag = oldflag;
    return 1;
}

int __cdecl Flush_Unused_File_Cache(int flush_keeps)
{
    int freed = 0;
    int index = 0;

    for (FileDataType *filedata = FileDataPtr; filedata->Name && strlen(filedata->Name); filedata = &FileDataPtr[index]) {

        if (filedata->Ptr && !filedata->OpenCount && (flush_keeps || !(filedata->Flag & 8192))) {
            Mem_Free(FileCacheHeap, filedata->Ptr);
            filedata->Ptr = NULL;
            ++freed;
        }

        ++index;
    }

    return freed;
}

int __cdecl Free_Resident_File(char *file)
{
    int fileindex = Find_File_Index(file);
    if (fileindex == -1) {
        return 0;
    }

    FileDataType *filedata = &FileDataPtr[fileindex];
    if (!filedata->Ptr) {
        return 1;
    }

    int oldflag = filedata->Flag;
    filedata->Flag &= ~9216u;
    filedata->Flag |= 2048u;
    int handle = Open_File(file, 1);
    Close_File(handle);
    filedata->Flag = oldflag;
    return 1;
}