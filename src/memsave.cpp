#include "defines.h"
#include "structs.h"
#include "file/file.h"

void __cdecl MemorySaveGameClass::Set_File_Pointer(int location)
{
    file_pointer = location;
}

int __cdecl MemorySaveGameClass::Write_Data(unsigned char *data, int bytes_of_data)
{
    Mem_Copy(data, &save_buffer[file_pointer], bytes_of_data);
    file_pointer += bytes_of_data;
    return bytes_of_data;
}

int __cdecl MemorySaveGameClass::Read_Data(unsigned char *data, int bytes_of_data)
{
    Mem_Copy(&save_buffer[file_pointer], data, bytes_of_data);
    file_pointer += bytes_of_data;
    return bytes_of_data;
}

void __cdecl MemorySaveGameClass::Write_Buffer_To_File(char *filename)
{
    int fh = Open_File(filename, 2);
    if (fh != -1) {
        Write_File(fh, save_buffer, sizeof(save_buffer));
        Close_File(fh);
    }
}