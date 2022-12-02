#include <cstdint>
int TickOffset = 0;

extern int32_t FUNC_004342CC();
uint32_t VQA_GetTime();

//done
void VQA_SetTimer(int32_t time, int32_t method)
{
    TickOffset = 0;
    uint32_t currtime = VQA_GetTime();
    TickOffset = time - currtime;
}

// done
uint32_t VQA_GetTime()
{
    return TickOffset + 3 * FUNC_004342CC() / 50;
}

//done
int32_t VQA_TimerMethod()
{
    return 1;
}