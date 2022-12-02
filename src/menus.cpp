#include "gamestat.h"
#include "globals.h"
#include "mcgaprim/gbuffer.h"

int Disabled_Menus_Level_Counter;

void Ungray_All_Grayed_Menus()
{
    HMENU menu = GetMenu(Main_Window_Handle);

    HMENU sub0 = GetSubMenu(menu, 0);
    EnableMenuItem(sub0, 102u, 0);

    HMENU sub1 = GetSubMenu(menu, 1);
    EnableMenuItem(sub1, 200u, 0);
    EnableMenuItem(sub1, 201u, 0);
    EnableMenuItem(sub1, 202u, 0);
    EnableMenuItem(sub1, 203u, 0);
    EnableMenuItem(sub1, 204u, 0);
    EnableMenuItem(sub1, 205u, 0);
}

void Disable_Main_Menu_Bar()
{
    if (/* !Game_Status.Query_Game_Over_Flag() &&*/ ++Disabled_Menus_Level_Counter <= 1) {
        HMENU menu = GetMenu(Main_Window_Handle);
        for (int i = 0; GetMenuItemCount(menu) > i; ++i) {
            EnableMenuItem(menu, i, MF_GRAYED | MF_BYPOSITION);
        }

        DrawMenuBar(Main_Window_Handle);
    }
}

int Enable_Main_Menu_Bar()
{
    if (--Disabled_Menus_Level_Counter < 0) {
        Disabled_Menus_Level_Counter = 0;
    }
    if (Disabled_Menus_Level_Counter > 0) {
        return 0;
    }

    HMENU menu = GetMenu(Main_Window_Handle);

    for (int i = 0; GetMenuItemCount(menu) > i; ++i) {
        EnableMenuItem(menu, i, MF_BYPOSITION);
    }

    DrawMenuBar(Main_Window_Handle);
    return 1;
}

void Gray_Or_Ungray_Menu_Item(int menu_bar_item, int sub_menu_item, unsigned int flag)
{
    EnableMenuItem(GetSubMenu(GetMenu(Main_Window_Handle), menu_bar_item), sub_menu_item, flag);
}

void FUNC_004208D5(int num)
{
    Disabled_Menus_Level_Counter = num;
}