#pragma once
#include "tempcode.h"

class CardsClass
{
public:
    void _cdecl Put_GOOJF_Chance_At_Deck_Bottom();
    void _cdecl Put_GOOJF_CC_At_Deck_Bottom();
    ChanceCardType _cdecl Query_Chance_Card(int deck_index);
    CommunityChestCardType _cdecl Query_CC_Card(int deck_index);

    void _cdecl Set_Chance_Card(int deck_index, ChanceCardType card);
    void _cdecl Set_CC_Card(int deck_index, CommunityChestCardType card);

    __cdecl CardsClass();
    void _cdecl New_Game_Reset();

private:
    ChanceCardType chance_cards[23];
    CommunityChestCardType community_chest_cards[21];
    unsigned char chance_deck_index;
    unsigned char community_chest_deck_index;
};
