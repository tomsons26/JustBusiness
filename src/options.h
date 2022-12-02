#pragma once
#include "defines.h"
typedef int BOOL;

class OptionsClass
{
public:
    __cdecl OptionsClass();

    void __cdecl OptionsClass::Set_Option(OptionGroupType option_group, int option, int value);
    BOOL __cdecl OptionsClass::Flip_Option(OptionGroupType option_group, int option);
    BOOL __cdecl OptionsClass::Get_Option(OptionGroupType option_group, int option);

    unsigned char site_options_bits[4];
    unsigned char debug_options_bits[1];
    unsigned char international_options_bits[2];
};
