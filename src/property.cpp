#include "property.h"

PropertyClass::PropertyClass(LocationType loc_id, PropertyType prop_id, ColorGroupType color,
    TitleDeedTextColorType deed_text_color, unsigned int prop_price, unsigned int base_rent, unsigned int one_house_rent,
    unsigned int two_house_rent, unsigned int three_house_rent, unsigned int four_house_rent, unsigned int hotel_rent,
    unsigned char mortgage, unsigned char housing_cost)
{
    location_id = loc_id;
    property_id = prop_id;
    color_group = color;
    title_deed_text_color = deed_text_color;
    price = prop_price;
    rent[0] = base_rent;
    rent[1] = one_house_rent;
    rent[2] = two_house_rent;
    rent[3] = three_house_rent;
    rent[4] = four_house_rent;
    rent[5] = hotel_rent;
    mortgage_value = mortgage;
    house_cost = housing_cost;
    player_owner = NO_PLAYER;
    internal_number_of_buildings = NO_HOUSE;
    mortgaged = 0;
}
