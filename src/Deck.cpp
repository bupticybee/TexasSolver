//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "include/Deck.h"

Deck::Deck() = default;

Deck::Deck(const vector<string>& ranks, const vector<string>& suits) {
    this->ranks = ranks;
    this->suits = suits;
    int card_num = 0;
    for(const string& one_rank : ranks){
        for(const string& one_suit: suits){
            string one_card = one_rank + one_suit;
            cards_str.push_back(one_card);
            cards.emplace_back(one_card,card_num);
            card_num += 1;
        }
    }
}

vector<Card>& Deck::getCards() {
    return this->cards;
}

