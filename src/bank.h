#pragma once

class BankClass
{
public:
    __cdecl BankClass();
    void __cdecl New_Game_Reset();

    void __cdecl Set_Houses(int num_houses);
    int __cdecl Get_Houses();

    void __cdecl Set_Hotels(int num_hotels);
    int __cdecl Get_Hotels();

private:
    int houses_in_bank;
    int hotels_in_bank;
};