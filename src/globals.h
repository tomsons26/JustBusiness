#pragma once
#include "mapview.h"
#include <cstddef>
#ifndef DLL_BUILD
extern unsigned int Monopoly_Start_Time;
extern int Is_Palette_Device;
extern int Is_Supported_Depth;
extern int System_Screen_Width;
extern int System_Screen_Height;
extern int System_Border_Width;
extern int System_Border_Height;
extern int System_Caption_Height;
extern int System_Menu_Bar_Height;

extern char Executable_Path[200];
extern char CD_Root_Path[200];

extern char Text_String[255];
extern char Text_String_1[255];
extern char Text_String_2[255];

extern char *Site_Receive_Message_Strings[6];

extern HINSTANCE Global_Instance_Handle;
extern HWND Main_Window_Handle;

extern char *String_TRR_Data;
extern char *Scores_String_TRR_Data;
extern char *AI_String_TRR_Data;
extern char *Facts_String_TRR_Data;
extern char *Deeds_String_TRR_Data;



extern int DIB_Top_Down;
extern int DIB_Bottom_Up;
extern HPALETTE Main_Palette_Handle;
extern MSG Global_Message_Queue;

#else
extern unsigned int &Monopoly_Start_Time;
extern int &Is_Palette_Device;
extern int &Is_Supported_Depth;
extern int &System_Screen_Width;
extern int &System_Screen_Height;
extern int &System_Border_Width;
extern int &System_Border_Height;
extern int &System_Caption_Height;
extern int &System_Menu_Bar_Height;

extern char *&Executable_Path;
extern char *&CD_Root_Path;

extern char *&Text_String;
extern char *&Text_String_1;
extern char *&Text_String_2;

extern char *&Site_Receive_Message_Strings;

extern HINSTANCE &Global_Instance_Handle;
extern HWND &Main_Window_Handle;

extern char *&String_TRR_Data;
extern char *&Scores_String_TRR_Data;
extern char *&AI_String_TRR_Data;
extern char *&Facts_String_TRR_Data;
extern char *&Deeds_String_TRR_Data;

extern int &DIB_Top_Down;
extern int &DIB_Bottom_Up;

extern HDC &WinG_DC;
extern HPALETTE &Main_Palette_Handle;
extern MSG &Global_Message_Queue;

#endif