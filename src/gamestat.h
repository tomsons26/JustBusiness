#pragma once
#include "players.h"

class GameStatusClass
{
public:
    __cdecl GameStatusClass();
    void __cdecl New_Game_Reset();

    void __cdecl Set_Game_In_Progress(int flag);
    int __cdecl Is_Game_In_Progress();

    PlayerType __cdecl Get_Whos_Turn_Is_It_Now();
    void __cdecl Set_Whos_Turn(PlayerType player);

    void __cdecl Set_Dice_Doubles_Count(int new_doubles_count);
    int __cdecl Get_Dice_Doubles_Count();

    void __cdecl Set_Free_Parking_Money(int cash);
    int __cdecl Get_Free_Parking_Money();

    void __cdecl Set_Who_Has_GOOJF_Chance_Card(PlayerType player);
    void __cdecl Set_Who_Has_GOOJF_CC_Card(PlayerType player);

    PlayerType __cdecl Get_Who_Has_GOOJF_Chance();
    PlayerType __cdecl Get_Who_Has_GOOJF_Community();

    void __cdecl Set_To_Post_Monopoly_Status();
    void __cdecl Reset_Post_Monopoly_Status();
    int __cdecl Is_Post_Monopoly_Status();

    char __cdecl Get_Total_Players();
    void __cdecl Set_Total_Players(PlayerType total_players);

    int _cdecl Board_Refresh_Mode_Active(BoardRefreshFlagType query_flag);
    void _cdecl Set_Board_Refresh_Mode(BoardRefreshFlagType query_flag);
    void _cdecl Reset_Board_Refresh_Mode(BoardRefreshFlagType query_flag);

    int __cdecl Query_Game_Option_Flag(unsigned char game_option);
    int __cdecl Set_Game_Option_Flag(unsigned char game_option);
    int __cdecl Reset_Game_Option_Flag(unsigned char game_option);

    int __cdecl Query_Secret_Option_Flag(unsigned char option);
    int __cdecl Set_Secret_Option_Flag(unsigned char option);
    int __cdecl Reset_Secret_Option_Flag(unsigned char option);

    void __cdecl Set_Game_Over_Flag(int flag);
    int __cdecl Query_Game_Over_Flag();

private:
    int game_in_progress;
    PlayerType whos_turn;
    int doubles_count;
    PlayerType who_has_goojf_chance;
    PlayerType who_has_goojf_community;
    int post_monopoly;
    int game_over;
    int free_parking_cash;
    unsigned char game_options_bits[2];
    unsigned char secret_options_bits[2];
    PlayerType total_players_in_game;
    int board_refresh_mode;
};
