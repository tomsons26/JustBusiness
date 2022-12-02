#include "file.h"
#include "filetemp.h"
#include "_file.h"

#include <cstdlib>
#include <cstring>
#include <direct.h>

#define _dos_getdrive(dd) (*(dd) = _getdrive())
#define _dos_setdrive(d, n) ((*(n) = _getdrive()), _chdrive((d)))

//borland wrapper
int getdisk(void)
{
    unsigned drive;

    _dos_getdrive(&drive);
    return ((int)drive - 1);
}

int __cdecl Find_File(char *filename)
{
    int disk;
    FileDataType *filedata;

    if (!filename) {
        return 0;
    }

    int index = Find_File_Index(filename);

    if (index != -1) {
        filedata = &FileDataPtr[index];
    } else {
        filedata = NULL;
    }

    if (index != -1 && filedata->Ptr) {
        return 1;
    }

    DiskNumber = -1;
    ibm_setdisk(StartPath[0] - 'A');
    ++CallingDOSInt;
    int handle = ibm_open(filename, 0, 256);
    --CallingDOSInt;

    if (handle != -1) {
        ++CallingDOSInt;
        ibm_close(handle);
        disk = getdisk();
        --CallingDOSInt;
        disk = disk + 1;
    } else if (index != -1 && filedata->Flag & 4096) {
        disk = Find_File(FileDataPtr[filedata->Disk].Name);
    } else if (!MultiDriveSearch) {
        disk = 0;
    } else {
        ibm_setdisk(DataPath[0] - 'A');
        Hard_Error_Occured = 0;
        int d = Open_File_With_Recovery(filename, 32768);
        if (d != -1) {
            ibm_close(d);
            disk = ibm_getdisk() + 1;
        } else {
            ibm_setdisk(StartPath[0] - 'A');
            disk = 0;
        }
    }
    return disk;
}


int __cdecl Comp_Func(const void *p1, const void *p2)
{
    return strcmp(*(const char **)p1, *(const char **)p2);
}

int __cdecl Find_File_Index(char *filename)
{
    FileDataType *filedata;
    char *key;

    if (filename
        && ((key = strupr(filename), !strstr(key, ".PAK")) ?
                (filedata = (FileDataType *)bsearch(
                     &key, &FileDataPtr[NumPAKFiles], NumFiles, sizeof(FileDataType), Comp_Func)) :
                FileData != FileDataPtr ?
                (filedata = (FileDataType *)bsearch(&key, FileDataPtr, NumPAKFiles, sizeof(FileDataType), Comp_Func)) :
                (filedata = (FileDataType *)lfind(&key, FileDataPtr, &NumPAKFiles, sizeof(FileDataType), Comp_Func)),
            filedata)) {
        return filedata - FileDataPtr;
    }

    return -1;
}
