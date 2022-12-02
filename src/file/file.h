#pragma once
#include <cstdint>

#define XMAXPATH 80

#ifndef READ
#define READ 1
#endif
#ifndef WRITE
#define WRITE 2
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

typedef enum : int8_t
{
    FI_SUCCESS = 0x00,
    FI_CACHE_TOO_BIG = 0x01,
    FI_CACHE_ALREADY_INIT = 0x02,
    FI_FILEDATA_FILE_NOT_FOUND = 0x04,
    FI_FILEDATA_TOO_BIG = 0x08,
    FI_SEARCH_PATH_NOT_FOUND = 0x10,
    FI_STARTUP_PATH_NOT_FOUND = 0x20,
    FI_NO_CACHE_FOR_PRELOAD = 0x40,
    FI_FILETABLE_NOT_INIT = 0x80,
} FileInitErrorType;

typedef enum : int8_t
{
    CANT_CREATE_FILE,
    BAD_OPEN_MODE,
    COULD_NOT_OPEN,
    TOO_MANY_FILES,
    CLOSING_NON_HANDLE,
    READING_NON_HANDLE,
    WRITING_NON_HANDLE,
    SEEKING_NON_HANDLE,
    SEEKING_BAD_OFFSET,
    WRITING_RESIDENT,
    UNKNOWN_INDEX,
    DID_NOT_CLOSE,
    FATAL_ERROR,
    FILE_NOT_LISTED,
    FILE_LENGTH_MISMATCH,
    INTERNAL_ERROR,
    MAKE_RESIDENT_ZERO_SIZE,
    RESIDENT_SORT_FAILURE,

    NUMBER_OF_ERRORS
} FileErrorType;

extern short (*Open_Error)(FileErrorType, char const *);

typedef struct
{
    char *Name;
    long Size;
    void *Ptr;
    long Start;
    unsigned char Disk;
    unsigned char OpenCount;
    unsigned int Flag;
} FileDataType;



extern FileDataType FileData[];
extern char ExecPath[XMAXPATH + 1];
extern char DataPath[XMAXPATH + 1];
extern char StartPath[XMAXPATH + 1];
extern int UseCD;


void __cdecl WWDOS_Shutdown();
FileInitErrorType __cdecl WWDOS_Init(unsigned long cachesize, char *filedata, char *cdpath);

int __cdecl Open_File(char *file_name, int mode);
void __cdecl Close_File(int handle);
long __cdecl Read_File(int handle, void *buf, unsigned long bytes);
long __cdecl Write_File(int handle, void *buf, unsigned long bytes);
unsigned int __cdecl Seek_File(int handle, long offset, int starting);
int __cdecl File_Exists(char *file_name);
int __cdecl File_Size(int handle);
int __cdecl Is_Handle_Valid(int handle, FileErrorType error, char const *name);
int __cdecl Open_File_With_Recovery(char const *file_name, unsigned int mode);

void Unfragment_File_Cache();
int __cdecl Make_File_Resident(char *filename);
int __cdecl Flush_Unused_File_Cache(int flush_keeps);
int __cdecl Free_Resident_File(char const *file);

int __cdecl Find_File(char *file_name);
int __cdecl Find_File_Index(char *filename);
