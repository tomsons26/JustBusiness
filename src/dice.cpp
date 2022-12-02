#include "dice.h"
int __cdecl DiceClass::Get_Die_1()
{
    return die_1;
}
int __cdecl DiceClass::Get_Die_2()
{
    return die_2;
}

int __cdecl DiceClass::Get_Total()
{
    return die_2 + die_1;
}

int __cdecl DiceClass::Currently_Doubles()
{
    return die_1 == die_2;
}

void __cdecl DiceClass::Set(int d1, int d2)
{
    die_1 = d1;
    die_2 = d2;
}

__cdecl DiceClass::DiceClass()
{
    New_Game_Reset();
}

void __cdecl DiceClass::New_Game_Reset()
{
    die_1 = 1;
    die_2 = 1;
}


void __cdecl DiceClass::Set_Debug_Die(int die_num, int val)
{
    ;
}