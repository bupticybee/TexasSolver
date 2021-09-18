//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/ranges/RiverCombs.h"

RiverCombs::RiverCombs() {

}

RiverCombs::RiverCombs(vector<int> board, PrivateCards private_cards, int rank, int reach_prob_index) {
    this->board = board;
    this->rank = rank;
    this->private_cards = private_cards;
    this->reach_prob_index = reach_prob_index;
}

