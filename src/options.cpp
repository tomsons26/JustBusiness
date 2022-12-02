
#include "options.h"
typedef int BOOL;

__cdecl OptionsClass::OptionsClass()
{
#if 0
    int v1 = 0;
    do {
        OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, v1++, 0);
    } while (v1 < 24);

    int v2 = 0;
    do {
        OptionsClass::Set_Option(OPTION_GROUP_INTERNATIONAL_OPTIONS, v2++, 0);
    } while (v2 < InternationalOptionType_count);

    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_wave, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_midi, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_computer_1, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_computer_2, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_computer_3, 1);

    int v3 = gameopt_anim_1;
    do {
        OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, v3++, 1);
    } while (v3 <= gameopt_anim_11);

    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_board_1, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_board_2, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_board_5, 1);
    OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, gameopt_board_6, 1);
#endif
}

void __cdecl OptionsClass::Set_Option(OptionGroupType option_group, int option, int value)
{
    int flag_byte; // esi
    char flag_bit; // dl

    flag_byte = option >> 3;
    flag_bit = option & 7;
    switch (option_group) {
        case OPTION_GROUP_SITE_OPTIONS:
            if (value) {
                site_options_bits[flag_byte] |= 1 << flag_bit;
            } else {
                site_options_bits[flag_byte] &= ~(1 << flag_bit);
            }
            break;
        case OPTION_GROUP_INTERNATIONAL_OPTIONS:
            if (value) {
                international_options_bits[flag_byte] |= 1 << flag_bit;
            } else {
                international_options_bits[flag_byte] &= ~(1 << flag_bit);
            }
            break;
        case OPTION_GROUP_DEBUG_OPTIONS:
            if (value) {
                debug_options_bits[flag_byte] |= 1 << flag_bit;
            } else {
                debug_options_bits[flag_byte] &= ~(1 << flag_bit);
            }
            break;
    }
}

BOOL __cdecl OptionsClass::Flip_Option(OptionGroupType option_group, int option)
{
    bool result; // eax

    if (option_group == 1) {
        if (OptionsClass::Get_Option(OPTION_GROUP_SITE_OPTIONS, option)) {
            OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, option, 0);
            result = 0;
        } else {
            OptionsClass::Set_Option(OPTION_GROUP_SITE_OPTIONS, option, 1);
            result = 1;
        }
    } else if (option_group == OPTION_GROUP_DEBUG_OPTIONS) {
        if (OptionsClass::Get_Option(OPTION_GROUP_DEBUG_OPTIONS, option)) {
            OptionsClass::Set_Option(OPTION_GROUP_DEBUG_OPTIONS, option, 0);
            result = 0;
        } else {
            OptionsClass::Set_Option(OPTION_GROUP_DEBUG_OPTIONS, option, 1);
            result = 1;
        }
    } else {
        result = 0;
    }
    return result;
}

BOOL __cdecl OptionsClass::Get_Option(OptionGroupType option_group, int option)
{
    int flag_byte; // ebx
    char flag_bit; // al

    flag_byte = option >> 3;
    flag_bit = option & 7;
    switch (option_group) {
        case OPTION_GROUP_SITE_OPTIONS:
            return (site_options_bits[flag_byte] & (1 << flag_bit)) != 0;
        case OPTION_GROUP_INTERNATIONAL_OPTIONS:
            return (international_options_bits[flag_byte] & (1 << flag_bit)) != 0;
        case OPTION_GROUP_DEBUG_OPTIONS:
            return (debug_options_bits[flag_byte] & (1 << flag_bit)) != 0;
    }
    return 0;
}