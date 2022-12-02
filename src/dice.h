#pragma once
typedef int BOOL;

class DiceClass
{
public:
    int __cdecl Get_Die_1();
    int __cdecl Get_Die_2();
    int __cdecl Get_Total();
    int __cdecl Currently_Doubles();

    void __cdecl Set(int d1, int d2);

    __cdecl DiceClass();

    void __cdecl New_Game_Reset();

    void __cdecl Set_Debug_Die(int die_num, int val);

private:
    int die_1;
    int die_2;
};
