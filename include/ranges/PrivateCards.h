//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PRIVATECARDS_H
#define TEXASSOLVER_PRIVATECARDS_H
#include "include/Card.h"

class PrivateCards {
public:
    int card1{};
    int card2{};
    float weight{};
    float relative_prob{};
    PrivateCards();
    PrivateCards(int card1, int card2, float weight);
    uint64_t toBoardLong();
    int hashCode();
    string toString();
    const vector<int> & get_hands() const;
private:
    vector<int> card_vec;
    int hash_code{};
    uint64_t board_long;
};


#endif //TEXASSOLVER_PRIVATECARDS_H
