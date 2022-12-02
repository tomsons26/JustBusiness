#pragma once

class MemorySaveGameClass
{
public:
    void __cdecl Set_File_Pointer(int location);
    int __cdecl Write_Data(unsigned char *data, int bytes_of_data);
    int __cdecl Read_Data(unsigned char *data, int bytes_of_data);
    void __cdecl Write_Buffer_To_File(char *filename);

private:
    int file_pointer;
    unsigned char save_buffer[1500];
};