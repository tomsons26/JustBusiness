#include "colorgrp.h"

int __cdecl ColorGroupClass::Get_Building_Cost()
{
    return building_cost;
}

int __cdecl ColorGroupClass::Get_Total_Properties()
{
    return total_properties;
}

int __cdecl ColorGroupClass::Get_Palette_Color()
{
    return palette_color;
}

int __cdecl ColorGroupClass::Get_Or_Set_Landing_Order(PropertyType property)
{
    int i = 0;
    do {
        if (property == properties[i]) {
            break;
        }
        ++i;
    } while (i < 3);

    if (landing_order[i]) {
        return landing_order[i];
    }

    int number_of_props_hit = 0;
    int j = 0;
    do {
        if (number_of_props_hit < landing_order[j]) {
            number_of_props_hit = landing_order[j];
        }
        ++j;
    } while (j < 3);

    landing_order[i] = number_of_props_hit + 1;

    return landing_order[i];
}
char Color_Group_Letter_Id[8] = {'p', 'a', 'm', 'o', 'r', 'y', 'g', 'b'};

char __cdecl ColorGroupClass::Get_Letter_Id()
{
    return Color_Group_Letter_Id[color_group_id];
}

void __cdecl ColorGroupClass::Get_Constituents(PropertyType *first_prop, PropertyType *second_prop, PropertyType *third_prop)
{
    *first_prop = properties[0];
    *second_prop = properties[1];
    *third_prop = properties[2];
}

ColorGroupClass::ColorGroupClass(ColorGroupType id, int bldg_cost, int total_props, int pal_color,
    PropertyType first_property, PropertyType second_property, PropertyType third_property)
{
    color_group_id = id;
    building_cost = bldg_cost;
    total_properties = total_props;
    palette_color = pal_color;
    properties[0] = first_property;
    properties[1] = second_property;
    properties[2] = third_property;
    landing_order[0] = 0;
    landing_order[0] = 0;
    landing_order[0] = 0;
}
