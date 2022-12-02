#pragma once

void __cdecl Real_Error_Window_Message(int message_string_num);
void __cdecl Real_Error_Window_Message(char *message);

unsigned int __cdecl Row_Based_RLE_Decompress_Top_Down(
    unsigned char *source, unsigned char *dest, unsigned int source_size, int surface_width, int row_width);
unsigned int __cdecl Row_Based_RLE_Decompress_Bottom_Up(
    unsigned char *source, unsigned char *dest, unsigned int source_size, int surface_width, int row_width);