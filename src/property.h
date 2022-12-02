#pragma once
#include "players.h"
#include "tempcode.h"

class PropertyClass
{
public:
    __cdecl PropertyClass(LocationType loc_id, PropertyType prop_id, ColorGroupType color,
        TitleDeedTextColorType deed_text_color, unsigned int prop_price, unsigned int base_rent, unsigned int one_house_rent,
        unsigned int two_house_rent, unsigned int three_house_rent, unsigned int four_house_rent, unsigned int hotel_rent,
        unsigned char mortgage, unsigned char housing_cost);

private:
    LocationType location_id;
    PropertyType property_id;
    ColorGroupType color_group;
    TitleDeedTextColorType title_deed_text_color;
    unsigned int price;
    unsigned int rent[6];
    unsigned char mortgage_value;
    unsigned char house_cost;
    PlayerType player_owner;
    BuildingStateType internal_number_of_buildings;
    int mortgaged;
};
