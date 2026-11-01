//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "ranges/RiverCombs.h"

RiverCombs::RiverCombs() {

}

RiverCombs::RiverCombs(PrivateCards private_cards, int rank, int reach_prob_index) {
    this->rank = rank;
    this->reach_prob_index = reach_prob_index;
    this->card1 = private_cards.card1;
    this->card2 = private_cards.card2;
    this->hand_long = private_cards.toBoardLong();
}
