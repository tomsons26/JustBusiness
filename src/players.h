/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief
 *
 * @copyright JustBusiness is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            3 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "defines.h"

struct PlayerClass
{
public:
    LocationType __cdecl Get_Board_Loc();
    void __cdecl Set_Board_Loc(LocationType loc);

    GameTokenType __cdecl Get_Token();
    void __cdecl Set_Token(GameTokenType set_token);

    void __cdecl New_Game_Reset(PlayerType id);

    __cdecl PlayerClass(PlayerType id);

    void __cdecl Set_Foo_Table(PlayerClass *a1, void *foo_tbl);
    float *__cdecl Get_Foo_Table();

    void __cdecl Set_Optimism(float opt);
    float __cdecl Get_Optimism();

    void __cdecl Set_Monopoly_Optimism(float opt);
    float __cdecl Get_Monopoly_Optimism();

    void __cdecl Set_Max_Remaining_Revolutions(int max_rem_rev);
    int __cdecl Get_Max_Remaining_Revolutions();

    void __cdecl Set_Average_Dollars_Per_Revolution(int dollars);
    int __cdecl Get_Average_Dollars_Per_Revolution();

    void __cdecl Set_Foo_Multiplier(float foo_mult);
    float __cdecl Get_Foo_Multiplier();

    void __cdecl Set_Single_RR_Multiplier(float single_RR_mult);
    float __cdecl Get_Single_RR_Multiplier();

    void __cdecl Set_Minimum_Desired_Cash_On_Hand(int amount);
    int __cdecl Get_Minimum_Desired_Cash_On_Hand();

    void __cdecl Set_Send_Property_To_Auction_Factor(float factor);
    float __cdecl Get_Send_Property_To_Auction_Factor();

    void __cdecl Set_Send_Property_To_Auction_Percentage(int percentage);
    int __cdecl Get_Send_Property_To_Auction_Percentage();

    void __cdecl Set_Minimum_Profit_Percentage(float percentage);
    float __cdecl Get_Minimum_Profit_Percentage();

    void __cdecl Set_Desired_Profit_Percentage(float percentage);
    float __cdecl Get_Desired_Profit_Percentage();

    void Set_Value_Of_Get_Out_Of_Jail_Free_Card(int value);
    int __cdecl Get_Value_Of_Get_Out_Of_Jail_Free_Card();

    void __cdecl Set_Low_Cash_Multiplier(float multiplier);
    float __cdecl Get_Low_Cash_Multiplier();

    void __cdecl Set_Cash_Value_Multiplier(float multiplier);
    float __cdecl Get_Cash_Value_Multiplier();

    void __cdecl Set_Non_Monopoly_Value_Multiplier(float multiplier);
    float __cdecl Get_Non_Monopoly_Value_Multiplier();

    void __cdecl Set_Monopoly_Value_Multiplier(float multiplier);
    float __cdecl Get_Monopoly_Value_Multiplier();

    void __cdecl Set_Houses_Value_Multiplier(float multiplier);
    float __cdecl Get_Houses_Value_Multiplier();

    void __cdecl Set_Percentage_Of_Time_Looks_For_Trade(int percentage);
    int __cdecl Get_Percentage_Of_Time_Looks_For_Trade();

    void __cdecl Set_Max_AI_Decision_Delay(int delay);
    int __cdecl Get_Max_AI_Decision_Delay();

    void __cdecl Set_Min_AI_Decision_Delay(int delay);
    int __cdecl Get_Min_AI_Decision_Delay();

    void __cdecl Set_Probability_Of_Bidding_High(int probability);
    int __cdecl Get_Probability_Of_Bidding_High();

    void __cdecl Set_Probability_Of_Bidding_Middle(int probability);
    int __cdecl Get_Probability_Of_Bidding_Middle();

    void __cdecl Set_Next_Auction_Bid_Time(int time);
    int __cdecl Get_Next_Auction_Bid_Time();

    void __cdecl Set_Max_Roll_Dice_Delay(int delay);
    int __cdecl Get_Max_Roll_Dice_Delay();

    void __cdecl Set_Offers_Trades_Value(int value);
    int __cdecl Get_Offers_Trades_Value();

private:
    PlayerType player_id;
    char name[30];
    char ip_address[17];
    int cash;
    LocationType board_loc;
    GameTokenType game_token;
    ActiveStatusType active_status;
    AIType ai_skill;
    int in_jail;
    int in_jail_roll_count;
    int laps_around_go_count;
    char immunity_counts[TOTAL_PROPERTIES];
    unsigned int next_auction_bid_time;
    int __beencheating;
    int __beenbustedcheating;
    float foo_table[TOTAL_LOCATIONS];
    float optimism;
    float monopoly_optimism;
    int max_remaining_revolutions;
    int average_dollars_per_revolution;
    float foo_multiplier;
    float single_RR_multiplier;
    int minimum_desired_cash_on_hand;
    float send_property_to_auction_factor;
    int send_property_to_auction_percentage;
    float minimum_profit_percentage;
    float desired_profit_percentage;
    int value_of_get_out_of_jail_free_card;
    float low_cash_multiplier;
    float cash_value_multiplier;
    float non_monopoly_value_multiplier;
    float monopoly_value_multiplier;
    float houses_value_multiplier;
    int percentage_of_time_looks_for_trade;
    int max_ai_decision_delay;
    int min_ai_decision_delay;
    int probability_of_bidding_high;
    int probability_of_bidding_middle;
    int max_roll_dice_delay;
    int offers_trades_value;
    int __always_buys_property;
    int __always_refuses_trade;
    int __always_gets_out_of_jail;
    int __always_stays_in_jail;
    int __never_bids_over_printed_price;
    int __build_only_houses;
    int __never_accepts_immunity;
    int __always_accepts_monopoly_in_trade;
};
