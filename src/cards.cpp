#include "defines.h"
#include "cards.h"

void __cdecl CardsClass::Put_GOOJF_Chance_At_Deck_Bottom()
{
    char i;
    char j;

    do {
        i = chance_deck_index++;
        j = chance_cards[i];
        if (chance_deck_index >= 23) {
            chance_deck_index = 0;
        }
    } while (j != 7);
    if (++chance_deck_index >= 23) {
        chance_deck_index = 0;
    }
}

void __cdecl CardsClass::Put_GOOJF_CC_At_Deck_Bottom()
{
    char i;
    char j;

    do {
        i = community_chest_deck_index++;
        j = community_chest_cards[i];
        if (community_chest_deck_index >= 21) {
            community_chest_deck_index = 0;
        }
    } while (j != 14);
    if (++community_chest_deck_index >= 21) {
        community_chest_deck_index = 0;
    }
}

ChanceCardType __cdecl CardsClass::Query_Chance_Card(int deck_index)
{
    return chance_cards[deck_index];
}

CommunityChestCardType __cdecl CardsClass::Query_CC_Card(int deck_index)
{
    return community_chest_cards[deck_index];
}

void __cdecl CardsClass::Set_Chance_Card(int deck_index, ChanceCardType card)
{
    chance_cards[deck_index] = card;
}

void __cdecl CardsClass::Set_CC_Card(int deck_index, CommunityChestCardType card)
{
    community_chest_cards[deck_index] = card;
}

__cdecl CardsClass::CardsClass()
{
    New_Game_Reset();
}

void __cdecl CardsClass::New_Game_Reset()
{
    chance_deck_index = 0;
    community_chest_deck_index = 0;
}
