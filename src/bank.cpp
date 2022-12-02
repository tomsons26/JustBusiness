#include "bank.h"

__cdecl BankClass::BankClass()
{
    BankClass::New_Game_Reset();
}

void __cdecl BankClass::New_Game_Reset()
{
    //if (GameStatusClass::Query_Game_Option_Flag(&Game_Status, GameOptionFlag_infinite_buildings_in_bank)) {
        houses_in_bank = 1032;
        hotels_in_bank = 1012;
    //} else {
        houses_in_bank = 32;
        hotels_in_bank = 12;
    //}
}

void __cdecl BankClass::Set_Houses(int num_houses)
{
    houses_in_bank = num_houses;
}

int __cdecl BankClass::Get_Houses()
{
    return houses_in_bank;
}

void __cdecl BankClass::Set_Hotels(int num_hotels)
{
    hotels_in_bank = num_hotels;
}

int __cdecl BankClass::Get_Hotels()
{
    return hotels_in_bank;
}