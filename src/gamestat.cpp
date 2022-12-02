#include "gamestat.h"
#include "players.h"

__cdecl GameStatusClass::GameStatusClass()
{
    New_Game_Reset();
}

void __cdecl GameStatusClass::New_Game_Reset()
{
    game_in_progress = 0;
    whos_turn = (PlayerType)-1;
    doubles_count = 0;
    who_has_goojf_chance = (PlayerType)-1;
    who_has_goojf_community = (PlayerType)-1;
    post_monopoly = 0;
    game_over = 0;
    free_parking_cash = 500;

    int v1 = 0;
    do {
        Reset_Secret_Option_Flag(v1++);
    } while (v1 < 13);

    total_players_in_game = PLAYER_1;
    board_refresh_mode = 0;
}

void __cdecl GameStatusClass::Set_Game_In_Progress(int flag)
{
    game_in_progress = flag;
}

int __cdecl GameStatusClass::Is_Game_In_Progress()
{
    return game_in_progress;
}

PlayerType __cdecl GameStatusClass::Get_Whos_Turn_Is_It_Now()
{
    return whos_turn;
}

void __cdecl GameStatusClass::Set_Whos_Turn(PlayerType player)
{
    whos_turn = player;
}

void __cdecl GameStatusClass::Set_Dice_Doubles_Count( int new_doubles_count)
{
     doubles_count = new_doubles_count;
}

int __cdecl GameStatusClass::Get_Dice_Doubles_Count()
{
    return doubles_count;
}

void __cdecl GameStatusClass::Set_Free_Parking_Money(int cash)
{
    free_parking_cash = cash;
}

int __cdecl GameStatusClass::Get_Free_Parking_Money()
{
    return free_parking_cash;
}

void __cdecl GameStatusClass::Set_Who_Has_GOOJF_Chance_Card(PlayerType player)
{
    who_has_goojf_chance = player;
}

void __cdecl GameStatusClass::Set_Who_Has_GOOJF_CC_Card(PlayerType player)
{
    who_has_goojf_community = player;
}

PlayerType __cdecl GameStatusClass::Get_Who_Has_GOOJF_Chance()
{
    return who_has_goojf_chance;
}

PlayerType __cdecl GameStatusClass::Get_Who_Has_GOOJF_Community()
{
    return who_has_goojf_community;
}

void __cdecl GameStatusClass::Set_To_Post_Monopoly_Status()
{
    post_monopoly = 1;
}

void __cdecl GameStatusClass::Reset_Post_Monopoly_Status()
{
    post_monopoly = 0;
}

int __cdecl GameStatusClass::Is_Post_Monopoly_Status()
{
    return post_monopoly;
}

char __cdecl GameStatusClass::Get_Total_Players()
{
    return total_players_in_game;
}

void __cdecl GameStatusClass::Set_Total_Players(PlayerType total_players)
{
    total_players_in_game = total_players;
}

int __cdecl GameStatusClass::Board_Refresh_Mode_Active(BoardRefreshFlagType query_flag)
{
    int ret_val = 0;
    if (board_refresh_mode & query_flag) {
        ret_val = 1;
    }
    return ret_val;
}

void __cdecl GameStatusClass::Set_Board_Refresh_Mode(BoardRefreshFlagType query_flag)
{
    board_refresh_mode |= query_flag;
    if (query_flag == BOARD_REFRESH_NORMAL) {
        board_refresh_mode = 0;
    }
}

void __cdecl GameStatusClass::Reset_Board_Refresh_Mode(BoardRefreshFlagType query_flag)
{
    board_refresh_mode &= ~query_flag;
}

int __cdecl GameStatusClass::Query_Game_Option_Flag(unsigned char game_option)
{
    return (game_options_bits[(int)game_option >> 3] & (1 << (game_option & 7))) != 0;
}

int __cdecl GameStatusClass::Set_Game_Option_Flag(unsigned char game_option)
{
    game_options_bits[(int)game_option >> 3] |= 1 << (game_option & 7);
    return 1;
}

int __cdecl GameStatusClass::Reset_Game_Option_Flag(unsigned char game_option)
{
    game_options_bits[(int)game_option >> 3] &= ~(1 << (game_option & 7));
    return 0;
}

int __cdecl GameStatusClass::Query_Secret_Option_Flag(unsigned char option)
{
    return (secret_options_bits[(int)option >> 3] & (1 << (option & 7))) != 0;
}

signed int __cdecl GameStatusClass::Set_Secret_Option_Flag(unsigned char option)
{
    secret_options_bits[(int)option >> 3] |= 1 << (option & 7);
    return 1;
}

int __cdecl GameStatusClass::Reset_Secret_Option_Flag(unsigned char option)
{
    secret_options_bits[(int)option >> 3] &= ~(1 << (option & 7));
    return 0;
}

void __cdecl GameStatusClass::Set_Game_Over_Flag(int flag)
{
    game_over = flag;
}

int __cdecl GameStatusClass::Query_Game_Over_Flag()
{
    return game_over;
}