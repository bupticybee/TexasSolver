//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_RIVERCOMBS_H
#define TEXASSOLVER_RIVERCOMBS_H


#include <ranges/PrivateCards.h>

class RiverCombs {
public:
    int rank;
    int reach_prob_index;
    int card1;
    int card2;
    uint64_t hand_long;
    RiverCombs();
    RiverCombs(PrivateCards private_cards , int rank, int reach_prob_index);
};


#endif //TEXASSOLVER_RIVERCOMBS_H
