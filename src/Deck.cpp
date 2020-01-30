//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "Deck.h"

Deck::Deck() {

}

Deck::Deck(vector<string> ranks, vector<string> suits) {
    this->ranks = ranks;
    this->suits = suits;
    for(string one_rank : ranks){
        for(string one_suit: suits){
            string one_card = one_rank + one_suit;
            cards_str.push_back(one_card);
            cards.push_back(Card(one_card));
        }
    }
}

vector<Card> Deck::getCards() {
    return this->getCards();
}

