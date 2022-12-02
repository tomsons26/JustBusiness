#include "players.h"

#include "tempcode.h"

LocationType PlayerClass::Get_Board_Loc()
{
    return board_loc;
}

void PlayerClass::Set_Board_Loc(LocationType loc)
{
    board_loc = loc;
}

GameTokenType __cdecl PlayerClass::Get_Token()
{
    return game_token;
}

void __cdecl PlayerClass::Set_Token(GameTokenType set_token)
{
    game_token = set_token;
}

void __cdecl PlayerClass::New_Game_Reset(PlayerType id)
{
    player_id = id;
    name[0] = 0;
    ip_address[0] = 0;
    cash = 1500;
    board_loc = LOC_FIRST;
    game_token = (GameTokenType)-1u;
    ai_skill = (AIType)-1u;
    active_status = PLAYER_INACTIVE;
    in_jail = 0;
    in_jail_roll_count = 0;
    laps_around_go_count = 0;

    for (int i = 0; i < TOTAL_PROPERTIES; ++i) {
        immunity_counts[i] = 0;
    };

    next_auction_bid_time = 0;
    __beencheating = 0;
    __beenbustedcheating = 0;

    foo_table[LOC_GO] = 18.07;
    foo_table[LOC_MEDITERRANEAN_AVENUE] = -18.65;
    foo_table[LOC_COMMUNITY_CHEST_A] = -17.780001;
    foo_table[LOC_BALTIC_AVENUE] = -17.309999;
    foo_table[LOC_INCOME_TAX] = -11.01;
    foo_table[LOC_READING_RAILROAD] = 13.27;
    foo_table[LOC_ORIENTAL_AVENUE] = -13.43;
    foo_table[LOC_CHANCE_A] = -11.64;
    foo_table[LOC_VERMONT_AVENUE] = -11.12;
    foo_table[LOC_CONNECTICUT_AVENUE] = -12.03;
    foo_table[LOC_JAIL] = 30.0;
    foo_table[LOC_ST_CHARLES_PLACE] = 3.53;
    foo_table[LOC_ELECTRIC_COMPANY] = 2.8199999;
    foo_table[LOC_STATES_AVENUE] = -10.46;
    foo_table[LOC_VIRGINIA_AVENUE] = -3.76;
    foo_table[LOC_PENNSYLVANIA_RAILROAD] = 9.1700001;
    foo_table[LOC_ST_JAMES_PLACE] = 7.5999999;
    foo_table[LOC_COMMUNITY_CHEST_B] = 9.9899998;
    foo_table[LOC_TENNESSEE_AVENUE] = 13.15;
    foo_table[LOC_NEW_YORK_AVENUE] = 16.200001;
    foo_table[LOC_FREE_PARKING] = 12.37;
    foo_table[LOC_KENTUCKY_AVENUE] = 7.0700002;
    foo_table[LOC_CHANCE_B] = 9.8699999;
    foo_table[LOC_INDIANA_AVENUE] = 4.1599998;
    foo_table[LOC_ILLINOIS_AVENUE] = 21.610001;
    foo_table[LOC_B_AND_O_RAILROAD] = 17.1;
    foo_table[LOC_ATLANTIC_AVENUE] = 3.21;
    foo_table[LOC_VENTNOR_AVENUE] = 2.1700001;
    foo_table[LOC_WATER_WORKS] = 7.25;
    foo_table[LOC_MARVIN_GARDENS] = -1.1;
    foo_table[LOC_GO_TO_JAIL] = 0.66000003;
    foo_table[LOC_PACIFIC_AVENUE] = 2.1500001;
    foo_table[LOC_NORTH_CAROLINA_AVENUE] = 0.16;
    foo_table[LOC_COMMUNITY_CHEST_C] = 3.28;
    foo_table[LOC_PENNSYLVANIA_AVENUE] = -4.5799999;
    foo_table[LOC_SHORT_LINE_RAILROAD] = -7.21;
    foo_table[LOC_CHANCE_C] = -11.9;
    foo_table[LOC_PARK_PLACE] = -16.49;
    foo_table[LOC_LUXURY_TAX] = -16.809999;
    foo_table[LOC_BOARDWALK] = 0.44;

    optimism = 0.025;
    monopoly_optimism = 0.079999998;

    max_remaining_revolutions = 60;
    average_dollars_per_revolution = 171;

    foo_multiplier = 2.0;
    single_RR_multiplier = 2.0;

    minimum_desired_cash_on_hand = 100;

    send_property_to_auction_factor = 1.0;
    send_property_to_auction_percentage = 50;

    minimum_profit_percentage = 1.0;
    desired_profit_percentage = 1.15;

    value_of_get_out_of_jail_free_card = 50;

    low_cash_multiplier = 1.5;
    cash_value_multiplier = 1.0;
    non_monopoly_value_multiplier = 1.1;
    monopoly_value_multiplier = 1.2;
    houses_value_multiplier = 2.5;

    percentage_of_time_looks_for_trade = 25;

    max_ai_decision_delay = 4;
    min_ai_decision_delay = 2;

    probability_of_bidding_high = 70;
    probability_of_bidding_middle = 90;

    max_roll_dice_delay = 3;
    offers_trades_value = 3;
    __always_buys_property = 0;
    __always_refuses_trade = 0;
    __always_gets_out_of_jail = 0;
    __always_stays_in_jail = 0;
    __never_bids_over_printed_price = 0;
    __build_only_houses = 0;
    __never_accepts_immunity = 0;
    __always_accepts_monopoly_in_trade = 0;
}

__cdecl PlayerClass::PlayerClass(PlayerType id)
{
    New_Game_Reset(id);
}

void __cdecl PlayerClass::Set_Foo_Table(PlayerClass *a1, void *foo_tbl)
{
    Mem_Copy((const char *)foo_tbl, foo_table, sizeof(foo_table));
}

float *__cdecl PlayerClass::Get_Foo_Table()
{
    return foo_table;
}

void __cdecl PlayerClass::Set_Optimism(float opt)
{
    if (opt < 0.0) {
        opt = 0.0;
    }
    if (opt > 5.0) {
        opt = 5.0;
    }
    optimism = opt;
}

float __cdecl PlayerClass::Get_Optimism()
{
    return optimism;
}

void __cdecl PlayerClass::Set_Monopoly_Optimism(float opt)
{
    if (opt < 0.0) {
        opt = 0.0;
    }
    if (opt > 2.0) {
        opt = 2.0;
    }

    monopoly_optimism = opt;
}

float __cdecl PlayerClass::Get_Monopoly_Optimism()
{
    return monopoly_optimism;
}

void __cdecl PlayerClass::Set_Max_Remaining_Revolutions(int max_rem_rev)
{
    if (max_rem_rev <= 55) {
        max_rem_rev = 56;
    }
    if (max_rem_rev > 100) {
        max_rem_rev = 100;
    }

    max_remaining_revolutions = max_rem_rev;
}

int __cdecl PlayerClass::Get_Max_Remaining_Revolutions()
{
    return max_remaining_revolutions;
}

void __cdecl PlayerClass::Set_Average_Dollars_Per_Revolution(int dollars)
{
    if (dollars <= 0) {
        dollars = 1;
    }
    if (dollars > 1000) {
        dollars = 1000;
    }

    average_dollars_per_revolution = dollars;
}

int __cdecl PlayerClass::Get_Average_Dollars_Per_Revolution()
{
    return average_dollars_per_revolution;
}

void __cdecl PlayerClass::Set_Foo_Multiplier(float foo_mult)
{
    if (foo_mult <= 0.0) {
        foo_mult = 0.0099999998;
    }
    if (foo_mult > 10.0) {
        foo_mult = 10.0;
    }

    foo_multiplier = foo_mult;
}

float __cdecl PlayerClass::Get_Foo_Multiplier()
{
    return foo_multiplier;
}

void __cdecl PlayerClass::Set_Single_RR_Multiplier(float single_RR_mult)
{
    if (single_RR_mult <= 0.0) {
        single_RR_mult = 0.0099999998;
    }
    if (single_RR_mult > 10.0) {
        single_RR_mult = 10.0;
    }

    single_RR_multiplier = single_RR_mult;
}

float __cdecl PlayerClass::Get_Single_RR_Multiplier()
{
    return single_RR_multiplier;
}

void __cdecl PlayerClass::Set_Minimum_Desired_Cash_On_Hand(int amount)
{
    if (amount < 0) {
        amount = 0;
    }
    if (amount > 1000) {
        amount = 1000;
    }

    minimum_desired_cash_on_hand = amount;
}

int __cdecl PlayerClass::Get_Minimum_Desired_Cash_On_Hand()
{
    return minimum_desired_cash_on_hand;
}

void __cdecl PlayerClass::Set_Send_Property_To_Auction_Factor(float factor)
{
    if (factor <= 0.0) {
        factor = 0.0099999998;
    }
    if (factor > 4.0) {
        factor = 4.0;
    }

    send_property_to_auction_factor = factor;
}

float __cdecl PlayerClass::Get_Send_Property_To_Auction_Factor()
{
    return send_property_to_auction_factor;
}

void __cdecl PlayerClass::Set_Send_Property_To_Auction_Percentage(int percentage)
{
    if (percentage < 0) {
        percentage = 0;
    }
    if (percentage > 100) {
        percentage = 100;
    }

    send_property_to_auction_percentage = percentage;
}

int __cdecl PlayerClass::Get_Send_Property_To_Auction_Percentage()
{
    return send_property_to_auction_percentage;
}

void __cdecl PlayerClass::Set_Minimum_Profit_Percentage(float percentage)
{
    if (percentage <= 0.0) {
        percentage = 0.0099999998;
    }
    if (percentage > 2.0) {
        percentage = 2.0;
    }

    minimum_profit_percentage = percentage;
}

float __cdecl PlayerClass::Get_Minimum_Profit_Percentage()
{
    return minimum_profit_percentage;
}

void __cdecl PlayerClass::Set_Desired_Profit_Percentage(float percentage)
{
    if (percentage < minimum_profit_percentage) {
        desired_profit_percentage = minimum_profit_percentage;
    } else if (percentage > 2.0) {
        desired_profit_percentage = 2.0;
    }
}

float __cdecl PlayerClass::Get_Desired_Profit_Percentage()
{
    return desired_profit_percentage;
}

void PlayerClass::Set_Value_Of_Get_Out_Of_Jail_Free_Card(int value)
{
    if (value < 0) {
        value = 0;
    }
    if (value > 60) {
        value = 60;
    }

    value_of_get_out_of_jail_free_card = value;
}

int __cdecl PlayerClass::Get_Value_Of_Get_Out_Of_Jail_Free_Card()
{
    return value_of_get_out_of_jail_free_card;
}

void __cdecl PlayerClass::Set_Low_Cash_Multiplier(float multiplier)
{
    if (multiplier <= 0.0) {
        multiplier = 0.0099999998;
    }
    if (multiplier > 10.0) {
        multiplier = 10.0;
    }

    low_cash_multiplier = multiplier;
}

float __cdecl PlayerClass::Get_Low_Cash_Multiplier()
{
    return low_cash_multiplier;
}

void __cdecl PlayerClass::Set_Cash_Value_Multiplier(float multiplier)
{
    if (multiplier <= 0.0) {
        multiplier = 0.0099999998;
    }
    if (multiplier > 10.0) {
        multiplier = 10.0;
    }

    cash_value_multiplier = multiplier;
}

float __cdecl PlayerClass::Get_Cash_Value_Multiplier()
{
    return cash_value_multiplier;
}

void __cdecl PlayerClass::Set_Non_Monopoly_Value_Multiplier(float multiplier)
{
    if (multiplier <= 0.0) {
        multiplier = 0.0099999998;
    }
    if (multiplier > 10.0) {
        multiplier = 10.0;
    }

    non_monopoly_value_multiplier = multiplier;
}

float __cdecl PlayerClass::Get_Non_Monopoly_Value_Multiplier()
{
    return non_monopoly_value_multiplier;
}

void __cdecl PlayerClass::Set_Monopoly_Value_Multiplier(float multiplier)
{
    if (multiplier <= 0.0) {
        multiplier = 0.0099999998;
    }
    if (multiplier > 10.0) {
        multiplier = 10.0;
    }

    monopoly_value_multiplier = multiplier;
}

float __cdecl PlayerClass::Get_Monopoly_Value_Multiplier()
{
    return monopoly_value_multiplier;
}

void __cdecl PlayerClass::Set_Houses_Value_Multiplier(float multiplier)
{
    if (multiplier <= 0.0) {
        multiplier = 0.0099999998;
    }
    if (multiplier > 10.0) {
        multiplier = 10.0;
    }

    houses_value_multiplier = multiplier;
}

float __cdecl PlayerClass::Get_Houses_Value_Multiplier()
{
    return houses_value_multiplier;
}

void __cdecl PlayerClass::Set_Percentage_Of_Time_Looks_For_Trade(int percentage)
{
    if (percentage < 0) {
        percentage = 0;
    }
    if (percentage > 100) {
        percentage = 100;
    }

    percentage_of_time_looks_for_trade = percentage;
}

int __cdecl PlayerClass::Get_Percentage_Of_Time_Looks_For_Trade()
{
    return percentage_of_time_looks_for_trade;
}

void __cdecl PlayerClass::Set_Max_AI_Decision_Delay(int delay)
{
    if (delay < 0) {
        delay = 0;
    }
    if (delay > 30) {
        delay = 30;
    }
    if (delay < min_ai_decision_delay) {
        delay = min_ai_decision_delay;
    }

    max_ai_decision_delay = delay;
}

int __cdecl PlayerClass::Get_Max_AI_Decision_Delay()
{
    return max_ai_decision_delay;
}

void __cdecl PlayerClass::Set_Min_AI_Decision_Delay(int delay)
{
    if (delay < 0) {
        delay = 0;
    }
    if (delay > 30) {
        delay = 30;
    }

    min_ai_decision_delay = delay;
}

int __cdecl PlayerClass::Get_Min_AI_Decision_Delay()
{
    return min_ai_decision_delay;
}

void __cdecl PlayerClass::Set_Probability_Of_Bidding_High(int probability)
{
    if (probability < 0) {
        probability = 0;
    }
    if (probability > 100) {
        probability = 100;
    }

    probability_of_bidding_high = probability;
}

int __cdecl PlayerClass::Get_Probability_Of_Bidding_High()
{
    return probability_of_bidding_high;
}

void __cdecl PlayerClass::Set_Probability_Of_Bidding_Middle(int probability)
{
    if (probability < 0) {
        probability = 0;
    }
    if (probability > 100) {
        probability = 100;
    }

    probability_of_bidding_middle = probability;
}

int __cdecl PlayerClass::Get_Probability_Of_Bidding_Middle()
{
    return probability_of_bidding_middle;
}

void __cdecl PlayerClass::Set_Next_Auction_Bid_Time(int time)
{
    next_auction_bid_time = time;
}

int __cdecl PlayerClass::Get_Next_Auction_Bid_Time()
{
    return next_auction_bid_time;
}

void __cdecl PlayerClass::Set_Max_Roll_Dice_Delay(int delay)
{
    if (delay < 1) {
        delay = 1;
    }
    if (delay > 20) {
        delay = 20;
    }

    max_roll_dice_delay = delay;
}

int __cdecl PlayerClass::Get_Max_Roll_Dice_Delay()
{
    return max_roll_dice_delay;
}

void __cdecl PlayerClass::Set_Offers_Trades_Value(int value)
{
    if (value < 0) {
        value = 0;
    }
    if (value > 6) {
        value = 6;
    }

    offers_trades_value = value;
}

int __cdecl PlayerClass::Get_Offers_Trades_Value()
{
    return offers_trades_value;
}