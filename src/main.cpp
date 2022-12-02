#include "globals.h"
#include "mcgaprim/gbuffer.h"

int Update_Windows_Messages()
{
    int ret_val = GetMessageA(&Global_Message_Queue, NULL, 0, 0);

    if (ret_val) {
        TranslateMessage(&Global_Message_Queue);
        DispatchMessageA(&Global_Message_Queue);
    }

    return ret_val;
}
