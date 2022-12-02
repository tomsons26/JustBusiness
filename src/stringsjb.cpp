#include "function.h"
#include "globals.h"
#include "mem/wwmem.h"

#include <cstdio>
#if 0
void Shutdown_Custom_Text_System()
{
    if (String_TRR_Data) {
        Free(String_TRR_Data);
    }
    if (AI_String_TRR_Data) {
        Free(AI_String_TRR_Data);
    }
    if (Scores_String_TRR_Data) {
        Free(Scores_String_TRR_Data);
    }
    if (Facts_String_TRR_Data) {
        Free(Facts_String_TRR_Data);
    }
    if (Deeds_String_TRR_Data) {
        Free(Deeds_String_TRR_Data);
    }
}

char *__cdecl Private_Get_TRR_Data_Pointer(char *trr_data, int line_num)
{
    if (!trr_data) {
        return "Error, No text data loaded";
    }

    char *v3 = trr_data + 2;
    unsigned short i = 0;
    while (line_num != *(unsigned short *)v3) {
        v3 += 2;
        if (*(unsigned short *)trr_data <= ++i) {
            sprintf(Text_String, "Unable to find line number in TRR data: %d", line_num);
            Real_Error_Window_Message(Text_String);
            return "String Error!";
        }
    }
    return &trr_data[*(unsigned short *)&trr_data[2 * *(unsigned short *)trr_data + 2 + 2 * i]];
}

char *__cdecl Get_TRR_Pointer(int line_num)
{
    return Private_Get_TRR_Data_Pointer(String_TRR_Data, line_num);
}

char *__cdecl Get_Score_Name_TRR_Pointer(int line_num)
{
    return Private_Get_TRR_Data_Pointer(Scores_String_TRR_Data, line_num);
}

char *__cdecl Get_AI_TRR_Pointer(int line_num)
{
    return Private_Get_TRR_Data_Pointer(AI_String_TRR_Data, line_num);
}

char *__cdecl Get_Fact_TRR_Pointer(int line_num)
{
    return Private_Get_TRR_Data_Pointer(Facts_String_TRR_Data, line_num);
}

char *__cdecl Get_Deed_TRR_Pointer(int line_num)
{
    return Private_Get_TRR_Data_Pointer((char *)Deeds_String_TRR_Data, line_num);
}
#endif