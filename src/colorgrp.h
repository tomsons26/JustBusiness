#pragma once
#include "defines.h"

class ColorGroupClass
{
public:
    int __cdecl Get_Building_Cost();
    int __cdecl Get_Total_Properties();
    int __cdecl Get_Palette_Color();
    int __cdecl Get_Or_Set_Landing_Order(PropertyType property);

    char __cdecl Get_Letter_Id();

    void __cdecl Get_Constituents(PropertyType *first_prop, PropertyType *second_prop, PropertyType *third_prop);

    __cdecl ColorGroupClass(ColorGroupType id, int bldg_cost, int total_props, int pal_color, PropertyType first_property,
        PropertyType second_property, PropertyType third_property);

private:
    ColorGroupType color_group_id;
    int building_cost;
    int total_properties;
    int palette_color;
    int landing_order[3];
    PropertyType properties[3];
};
