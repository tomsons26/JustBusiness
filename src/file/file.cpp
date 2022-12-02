#include "filetemp.h"
#include "_file.h"
#include "mapview.h"
#include <cstddef>
#include "..\mem\wwmem.h"

int __cdecl Open_File(char *file_name, int mode)
{
    FileDataType *filedata;
    int ii;
    char name[16];
    int i;
    int offset;
    FileDataType *parent;
    int local_mode;
    int handle;

    ++CallingDOSInt;
    FileHandleType *filehandletable = FileHandleTable;
    int file_handle = 0;

    do {
        if (filehandletable->Empty) {
            break;
        }
        ++file_handle;
        ++filehandletable;
    } while (file_handle < TABLE_MAX);

    if (file_handle == TABLE_MAX) {
        Do_IO_Error(TOO_MANY_FILES, file_name);
    }

    int index = Find_File_Index(file_name);
    if (index != -1) {
        filedata = &FileDataPtr[index];
    } else {
        filedata = FileDataPtr;
    }

    ibm_setdisk(StartPath[0] - 'A');

    int immediate = 0;

    if (index != -1) {
        if (filedata->Flag & 4096) {
            if (!filedata->Ptr) {
                int handle2 = ibm_open(file_name, 32768, 256);
                if (handle2 != -1) {
                    ibm_close(handle2);
                    immediate = 1;
                }
            }
        }
    }
    if (mode & 2 && index != -1 && filedata->Ptr) {

        if (!(filedata->Flag & 4096)) {
            Do_IO_Error(WRITING_RESIDENT, file_name);
        }

        Mem_Free(FileCacheHeap, filedata->Ptr);
        filedata->Ptr = 0;
    }
    if (filedata->Ptr && index != -1) {
        filehandletable->Index = index;
        filehandletable->Empty = 0;
        filehandletable->Pos = 0;
        filehandletable->Name = filedata->Name;
        filehandletable->Handle = 0;
        filehandletable->Mode = mode;

        if (filedata->Flag & 4096) {
            parent = &FileDataPtr[filedata->Disk];
        } else {
            parent = 0;
        }

        if (parent && parent->Ptr == filedata->Ptr) {
            filehandletable->Start = filedata->Start;
            ++parent->OpenCount;
        } else {
            filehandletable->Start = 0;
        }
        Mem_In_Use(filedata->Ptr);

    } else if (index != -1 && filedata->Flag & 4096 && !immediate && !(mode & 2)) {
        parent = &FileDataPtr[filedata->Disk];
        file_handle = Open_File(parent->Name, mode);
        parent = &FileDataPtr[filedata->Disk];
        filehandletable = &FileHandleTable[file_handle];

        if (FileData == FileDataPtr && !(FileDataPtr[filehandletable->Index].Flag & 256)) {
            FileDataType *last = NULL;
            Read_File(file_handle, &offset, sizeof(offset));
            while (offset) {
                i = 0;

                do {
                    Read_File(file_handle, &name[i], sizeof(char));
                    ii = i++;
                } while (name[ii]);
                i = Find_File_Index(name);

                if (i != -1) {
                    FileDataType *cur = &FileDataPtr[i];
                    if (cur->Flag & 4096) {
                        if (cur->Disk == filehandletable->Index) {
                            cur->Start = offset + FileDataPtr[filehandletable->Index].Start;
                            if (last) {
                                last->Size = cur->Start - last->Start;
                            }
                            last = cur;
                        }
                    }
                }
                Read_File(file_handle, &offset, sizeof(offset));
            }

            if (last) {
                last->Size = File_Size(file_handle) - last->Start;
            }
            FileDataPtr[filehandletable->Index].Flag |= 256;
        }

        filedata->Ptr = parent->Ptr;
        filehandletable->Index = index;
        filehandletable->Name = filedata->Name;
        filehandletable->Start = filedata->Start;
        Seek_File(file_handle, 0, 0);
        Cache_File(index, file_handle);
    } else {

        local_mode = 32768;
        if (mode == 1) {
            local_mode = 32768;
        } else if (mode != 2) {
            if (mode != 3) {
                Do_IO_Error(BAD_OPEN_MODE, file_name);
            }
            local_mode = 32770;
        } else {
            local_mode = 33537;
        }

        if (mode & 1 && !File_Exists(file_name)) {
            --CallingDOSInt;
            return -1;
        }

        while (true) {
            handle = Open_File_With_Recovery(file_name, local_mode);
            if (handle != -1) {
                break;
            }
            ibm_chdir(DataPath);
            ibm_chdir(StartPath);
            if (!Do_Open_Error(COULD_NOT_OPEN, file_name)) {
                --CallingDOSInt;
                return -1;
            }
        }

        if (immediate && index != -1) {
            filedata->Flag &= ~4096;
            filedata->Size = 0;
        }

        if (index != -1) {
            filedata->Size = ibm_lseek(handle, 0, 2);
            ibm_lseek(handle, 0, 0);
        }

        filehandletable->Index = index;
        filehandletable->Pos = 0;
        filehandletable->Start = 0;
        filehandletable->Empty = 0;
        filehandletable->Handle = handle;
        filehandletable->Mode = mode;

        if (index == -1) {
            filehandletable->Name = NULL;
        } else {
            filehandletable->Name = filedata->Name;
        }
        if (!(mode & 2)) {
            Cache_File(index, file_handle);
        }
    }

    if (index != -1) {
        ++filedata->OpenCount;
    }

    --CallingDOSInt;
    return file_handle;
}

void __cdecl Close_File(int handle)
{
    int index;
    FileDataType *filedata;
    int stillopen;

    signed int flushed = 0;
    FileDataType *parent = 0;

    if (Is_Handle_Valid(handle, CLOSING_NON_HANDLE, NULL)) {
        ++CallingDOSInt;
        index = FileHandleTable[handle].Index;
        if (index != -1) {
            filedata = &FileDataPtr[index];
        } else {
            filedata = NULL;
        }

        if (index != -1) {
            stillopen = --filedata->OpenCount;
            if (filedata->Flag & 4096) {
                parent = &FileDataPtr[filedata->Disk];
                if (parent->Ptr == filedata->Ptr) {
                    stillopen = --parent->OpenCount;
                }
            }
        }

        if (index != -1 && filedata->Ptr) {
            void *memptr = filedata->Ptr;

            if (parent && parent->Ptr == filedata->Ptr) {
                if (!filedata->OpenCount) {
                    filedata->Ptr = 0;
                }
                if (!parent->OpenCount && parent->Flag & 2048) {
                    Mem_Free(FileCacheHeap, parent->Ptr);
                    parent->Ptr = 0;
                    flushed = 1;
                }
            } else if (filedata->Flag & 2048 && !filedata->OpenCount) {
                Mem_Free(FileCacheHeap, filedata->Ptr);
                filedata->Ptr = 0;
                flushed = 1;
            }

            if (!flushed && !stillopen && !(filedata->Flag & 24576)) {
                Mem_Reference(memptr);
            }
        } else {
            ibm_close(FileHandleTable[handle].Handle);
        }

        FileHandleTable[handle].Empty = 1;
        --CallingDOSInt;
    }
}

long __cdecl Read_File(int handle, void *buf, unsigned long bytes)
{
    int v7;
    int v8;
    int number;
    int v10;
    FileDataType *filedata;

    char *v4 = (char *)buf;

    if (!buf || !bytes || !Is_Handle_Valid(handle, READING_NON_HANDLE, NULL)) {
        return 0;
    }

    ++CallingDOSInt;
    int doshandle = FileHandleTable[handle].Handle;
    int fileindex = FileHandleTable[handle].Index;
    if (fileindex != -1) {
        filedata = &FileDataPtr[fileindex];
        if (FileDataPtr[fileindex].Size) {
            if (bytes > filedata->Size - FileHandleTable[handle].Pos) {
                v7 = filedata->Size - FileHandleTable[handle].Pos;
            } else {
                v7 = bytes;
            }
            bytes = v7;
        }
    }
    unsigned int bytesread = 0;
    if (bytes) {
        if (doshandle) {
            for (; bytes; v4 += number) {
                Hard_Error_Occured = 0;
                if (bytes < 65520) {
                    v8 = bytes;
                } else {
                    v8 = 65520;
                }
                number = ibm_read(doshandle, v4, v8);
                if (Hard_Error_Occured) {
                    if (bytes < 65520) {
                        v10 = bytes;
                    } else {
                        v10 = 65520;
                    }
                    number = Read_File_With_Recovery(handle, v4, v10);
                    doshandle = FileHandleTable[handle].Handle;
                }
                bytes -= number;
                bytesread += number;
                FileHandleTable[handle].Pos += number;
                if (number < 65520) {
                    break;
                }
            }
        } else {
            Mem_Copy((char *)filedata->Ptr + FileHandleTable[handle].Start + FileHandleTable[handle].Pos, buf, bytes);
            bytesread = bytes;
            FileHandleTable[handle].Pos += bytes;
        }
    }
    --CallingDOSInt;
    return bytesread;
}

long __cdecl Write_File(int handle, void *buf, unsigned long bytes)
{
    FileDataType *filedata;
    int v7;
    int v9;

    int v3 = bytes;
    char *v4 = (char *)buf;
    if (!Is_Handle_Valid(handle, WRITING_NON_HANDLE, NULL)) {
        return 0;
    }
    int doshandle = FileHandleTable[handle].Handle;
    int fileindex = FileHandleTable[handle].Index;
    if (fileindex >= 0) {
        filedata = &FileDataPtr[fileindex];
    } else {
        filedata = 0;
    }
    if (!doshandle && filedata) {
        Do_IO_Error(WRITING_RESIDENT, filedata->Name);
    }
    ++CallingDOSInt;
    int byteswritten = 0;
    if (bytes) {
        do {
            if (v3 < 65520) {
                v7 = v3;
            } else {
                v7 = 65520;
            }
            int outbytes = ibm_write(doshandle, v4, v7);
            v3 -= outbytes;
            byteswritten += outbytes;
            FileHandleTable[handle].Pos += outbytes;
            if (!outbytes) {
                break;
            }
            if (fileindex != -1 && filedata) {
                if (filedata->Size <= FileHandleTable[handle].Pos) {
                    v9 = FileHandleTable[handle].Pos;
                } else {
                    v9 = filedata->Size;
                }
                filedata->Size = v9;
            }
            v4 += outbytes;
        } while (v3);
    }
    --CallingDOSInt;
    return byteswritten;
}

unsigned int __cdecl Seek_File(int handle, long offset, int starting)
{
    FileDataType *filedata; // edi
    int v5; // edx
    int v6; // eax
    int v7; // edx
    int v8; // edx
    int v9; // ecx

    if (!Is_Handle_Valid(handle, SEEKING_NON_HANDLE, NULL)) {
        return 0;
    }

    ++CallingDOSInt;
    int fileindex = FileHandleTable[handle].Index;
    int doshandle = FileHandleTable[handle].Handle;

    if (fileindex >= 0) {
        filedata = &FileDataPtr[fileindex];
    } else {
        filedata = NULL;
    }

    if (!doshandle) {

        if ((unsigned int)starting < 1) {
            if (offset < 0) {
                v5 = 0;
            } else {
                v5 = offset;
            }
            offset = v5;
            FileHandleTable[handle].Pos = 0;
        } else if (starting != 1) {
            if (starting != 2) {
                Do_IO_Error(SEEKING_BAD_OFFSET, FileHandleTable[handle].Name);
            }
            if (offset > 0) {
                v6 = 0;
            } else {
                v6 = offset;
            }
            offset = v6;
            FileHandleTable[handle].Pos = filedata->Size;
        }
        FileHandleTable[handle].Pos += offset;

        if (FileHandleTable[handle].Pos < filedata->Size - 1) {
            v7 = FileHandleTable[handle].Pos;
        } else {
            v7 = filedata->Size - 1;
        }

        FileHandleTable[handle].Pos = v7;
        if (FileHandleTable[handle].Pos > 0) {
            v8 = FileHandleTable[handle].Pos;
        } else {
            v8 = 0;
        }

        FileHandleTable[handle].Pos = v8;
        --CallingDOSInt;
        return FileHandleTable[handle].Pos;
    }

    if (fileindex == -1 || !(filedata->Flag & 4096)) {
        FileHandleTable[handle].Pos = ibm_lseek(doshandle, offset, starting);
        if (fileindex != -1) {
            FileHandleTable[handle].Pos -= FileHandleTable[handle].Start;
        }
        --CallingDOSInt;
        return FileHandleTable[handle].Pos;
    }

    if ((unsigned int)starting < 1) {
        FileHandleTable[handle].Pos = offset;
    } else if (starting != 1) {

        if (starting != 2) {
            Do_IO_Error(SEEKING_BAD_OFFSET, FileHandleTable[handle].Name);
        }

        if (offset > 0) {
            v9 = 0;
        } else {
            v9 = offset;
        }

        if (-v9 > filedata->Size) {
            Do_IO_Error(SEEKING_BAD_OFFSET, FileHandleTable[handle].Name);
        }
        FileHandleTable[handle].Pos = v9 + filedata->Size;
    } else {
        FileHandleTable[handle].Pos += offset;
        if (FileHandleTable[handle].Pos < 0) {
            FileHandleTable[handle].Pos = 0;
        }
        if (FileHandleTable[handle].Pos >= filedata->Size) {
            FileHandleTable[handle].Pos = filedata->Size;
        }
    }
    ibm_lseek(doshandle, FileHandleTable[handle].Start + FileHandleTable[handle].Pos, 0);
    --CallingDOSInt;
    return FileHandleTable[handle].Pos;
}

int __cdecl File_Exists(char *file_name)
{
    if (!file_name) {
        return 0;
    }

    if (Find_File(file_name)) {
        return 1;
    }

    while (Do_Open_Error(COULD_NOT_OPEN, file_name)) {
        ibm_setdisk(DataPath[0] - 'A');

        if (!ibm_chdir(DataPath)) {
            if (Find_File(file_name)) {
                return 1;
            }
        }
    }

    return 0;
}

int __cdecl File_Size(int handle)
{
    if (FileHandleTable[handle].Index != -1) {
        FileDataType *filedata = &FileDataPtr[FileHandleTable[handle].Index];
        if (filedata->Size) {
            return filedata->Size;
        }
    }

    int current = _llseek(FileHandleTable[handle].Handle, 0, 0);
    int end = _llseek(FileHandleTable[handle].Handle, 0, 2);
    _llseek(FileHandleTable[handle].Handle, current, 0);

    return end;
}

int __cdecl Is_Handle_Valid(int handle, FileErrorType error, char const *name)
{
    if (handle < 0 || handle >= TABLE_MAX) {
        Do_IO_Error(error, name);
    }

    if (FileHandleTable[handle].Empty) {
        Do_IO_Error(error, name);
    }

    return 1;
}
