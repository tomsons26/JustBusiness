#pragma once
#include "file.h"

#define TABLE_MAX 20

typedef struct
{
    int Empty;
    int Handle;
    long Pos;
    long Start;
    int Index;
    int Mode;
    char *Name;
} FileHandleType;

int ibm_getdisk();
int ibm_setdisk(int drive);
int ibm_close(int handle);
int ibm_unlink(char const *name);
int ibm_lseek(int handle, long offset, int where);
unsigned int ibm_read(int handle, void *ptr, unsigned int bytes);
unsigned int ibm_write(int handle, void *ptr, unsigned int bytes);
int ibm_open(char const *name, unsigned int mode, int attrib);
int ibm_chdir(char const *path);

int __cdecl Do_Open_Error(FileErrorType errormsgnum, char const *file_name);
void __cdecl Do_IO_Error(FileErrorType errormsgnum, char const *filename);
long __cdecl Read_File_With_Recovery(int handle, void *buf, unsigned int bytes);
int __cdecl Open_File_With_Recovery(char const *file_name, unsigned int mode);
int __cdecl Cache_File(int index, int file_handle);

#ifdef __cplusplus
extern "C" {
#endif

extern void Get_Devices();

#ifdef __cplusplus
}
#endif

extern char CallingDOSInt;
extern char MaxDevice;
extern char DefaultDrive;
extern char MultiDriveSearch;
extern FileDataType *FileDataPtr;
extern FileHandleType FileHandleTable[TABLE_MAX];
extern unsigned int NumFiles;
extern unsigned int NumPAKFiles;
extern void *FileCacheHeap;
extern int DiskNumber;
extern int MaxDirNum;
