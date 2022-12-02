
#include "_file.h"
#include "..\mem\wwmem.h"

#include <cstdlib>
#include <direct.h>
#include <string.h>
FileInitErrorType __cdecl Init_File_Cache(unsigned int cachesize);
FileInitErrorType __cdecl Init_FileData_Table(char *filename);
FileInitErrorType __cdecl Set_Search_Drives(char *cdpath);
FileInitErrorType __cdecl Preload_Files();
char *Find_Argv(char const *str);
void Sort_FileData_Table();

inline FileInitErrorType operator|=(FileInitErrorType a, FileInitErrorType b)
{
    return static_cast<FileInitErrorType>(static_cast<int>(a) | static_cast<int>(b));
}

inline FileInitErrorType operator|(FileInitErrorType a, FileInitErrorType b)
{
    return static_cast<FileInitErrorType>(static_cast<int>(a) | static_cast<int>(b));
}

FileInitErrorType __cdecl WWDOS_Init(unsigned long cachesize, char *filedata, char *cdpath)
{
    {
        FileInitErrorType errors;

        Get_Devices();
        if (cachesize) {
            errors = Init_File_Cache(cachesize);
        } else {
            errors = FI_SUCCESS;
        }
        errors |= Init_FileData_Table(filedata);
        errors |= Set_Search_Drives(cdpath);
        errors |= Preload_Files();
        return errors;
    }
}

void __cdecl WWDOS_Shutdown()
{
    FileHandleType *filehandletable = FileHandleTable;
    int file_handle = 0;

    do {
        if (!filehandletable->Empty) {
            Close_File(file_handle);
        }
        ++file_handle;
        ++filehandletable;
    } while (file_handle < TABLE_MAX);

    if (FileCacheHeap) {

        FileDataType *filedata;

        if (FileDataPtr) {
            filedata = FileDataPtr;
        } else {
            filedata = FileData;
        }

        while (filedata->Name && *filedata->Name) {
            filedata->Ptr = 0;
            ++filedata;
        }

        Free(FileCacheHeap);
        FileCacheHeap = 0;
    }
    if (FileData != FileDataPtr) {
        Free(FileDataPtr);
    }
    FileDataPtr = 0;

    chdir(StartPath);

    ibm_setdisk(StartPath[0] - 'A');
}

FileInitErrorType __cdecl Init_File_Cache(unsigned int cachesize)
{
    if (FileCacheHeap) {
        return FI_CACHE_ALREADY_INIT;
    }

    if (cachesize > Ram_Free(MEM_NORMAL)) {
        return FileCacheHeap == 0 ? FI_SUCCESS : FI_CACHE_TOO_BIG;
    }

    FileCacheHeap = Alloc(cachesize, MEM_NORMAL);
    Mem_Init(FileCacheHeap, cachesize);

    return FileCacheHeap == 0 ? FI_SUCCESS : FI_CACHE_TOO_BIG;
}

FileInitErrorType __cdecl Init_FileData_Table(char *filename)
{
    int fd; // esi
    unsigned int fsize; // edi
    FileDataType *ptr; // ebx

    int index = 0;
    do {
        FileHandleTable[index++].Empty = 1;
    } while (index < TABLE_MAX);

    FileDataPtr = FileData;

    Sort_FileData_Table();

    if (!filename) {
        return FI_SUCCESS;
    }

    if (!Find_File(filename)) {
        return FI_FILEDATA_FILE_NOT_FOUND;
    }

    fd = Open_File(filename, 1);
    fsize = File_Size(fd);

    if (fsize > Ram_Free(MEM_NORMAL)) {
        Close_File(fd);
        return FI_FILEDATA_TOO_BIG;
    }

    ptr = (FileDataType *)Alloc(fsize, MEM_NORMAL);
    FileDataPtr = ptr;

    Read_File(fd, ptr, fsize);
    Close_File(fd);

    NumFiles = 0;
    NumPAKFiles = 0;

    while (ptr->Name) {
        ptr->Name += (unsigned int)FileDataPtr;

        if (!NumFiles && strstr(ptr->Name, ".PAK")) {
            ++NumPAKFiles;
            ptr->Flag |= 0x100u;
        } else {
            ++NumFiles;
        }

        ++ptr;
    }

    return FI_SUCCESS;
}

FileInitErrorType __cdecl Set_Search_Drives(char *cdpath)
{
    ExecPath[0] = 0;
    char *ptr = Find_Argv("-CD");
    if (ptr) {
        strcpy(DataPath, ptr + 3);
    } else {
        if (cdpath && *cdpath) {
            UseCD = 0;
        } else {
            UseCD = 0;
        }
        if (UseCD) {
            strcpy(DataPath, "A:");
            strcat(DataPath, cdpath);
            DataPath[0] = UseCD + 'A';
        } else {
            strcpy(DataPath, ExecPath);
        }
    }
    getcwd(StartPath, 80);
    strupr(StartPath);
    strupr(DataPath);
    strupr(ExecPath);
    if (DataPath[0] && chdir(DataPath)) {
        return FI_SEARCH_PATH_NOT_FOUND;
    }
    if (chdir(StartPath)) {
        return FI_STARTUP_PATH_NOT_FOUND;
    }
    return FI_SUCCESS;
}

FileInitErrorType __cdecl Preload_Files()
{
    if (!FileDataPtr) {
        return FI_FILETABLE_NOT_INIT;
    }
    if (!FileCacheHeap) {
        return FI_NO_CACHE_FOR_PRELOAD;
    }
    for (FileDataType *filedata = FileDataPtr; filedata->Name && strlen(filedata->Name); ++filedata) {
        if (filedata->Flag & 512) {
            int oldflag = filedata->Flag;
            filedata->Flag |= 1024;
            filedata->Flag &= ~2048;
            int handle = Open_File(filedata->Name, 1);
            Close_File(handle);
            filedata->Flag &= ~3072;
            filedata->Flag |= oldflag & 3072;
        }
    }
    return FI_SUCCESS;
}

int __cdecl QSort_Comp_Func(const void *a1, const void *a2)
{
    return strcmp(*(const char **)a1, *(const char **)a2);
}

void Sort_FileData_Table()
{
    NumPAKFiles = 0;
    strupr(FileData[0].Name);
    while (strstr(FileData[NumPAKFiles].Name, ".PAK")) {
        strupr(FileData[NumPAKFiles].Name);
        ++NumPAKFiles;
    }
    for (NumFiles = 0; FileData[NumPAKFiles + NumFiles].Name && FileData[NumPAKFiles + NumFiles].Name[0]; ++NumFiles) {
        strupr(FileData[NumPAKFiles + NumFiles].Name);
    }
    if (NumFiles) {
        qsort(&FileData[NumPAKFiles], NumFiles, sizeof(FileDataType), QSort_Comp_Func);
    }
}

void Get_Devices()
{
    MaxDevice = 0;
    DefaultDrive = 0;
}
