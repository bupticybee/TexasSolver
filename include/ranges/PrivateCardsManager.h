//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PRIVATECARDSMANAGER_H
#define TEXASSOLVER_PRIVATECARDSMANAGER_H
#include "PrivateCards.h"

class PrivateCardsManager {
private:
    vector<vector<PrivateCards>> private_cards;
    int player_number;
    uint64_t initialboard;
    vector<vector<int>> card_player_index;
public:
    PrivateCardsManager();
    PrivateCardsManager(vector<vector<PrivateCards>> private_cards,int player_number,uint64_t initialboard);
    vector<PrivateCards>& getPreflopCards(int player);
    int indPlayer2Player(int from_player,int to_player,int index);
    vector<float> getInitialReachProb(int player,uint64_t initialboard);
    void setRelativeProbs();
};

#endif //TEXASSOLVER_PRIVATECARDSMANAGER_H
