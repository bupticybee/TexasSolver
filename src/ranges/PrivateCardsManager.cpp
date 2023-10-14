//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/ranges/PrivateCardsManager.h"

PrivateCardsManager::PrivateCardsManager() {

}

PrivateCardsManager::PrivateCardsManager(vector<vector<PrivateCards>> private_cards, int player_number,
                                         uint64_t initialboard) {
    this->private_cards = private_cards;
    this->player_number = player_number;
    this->card_player_index = vector<vector<int>>(52 * 52);
    for(int i = 0;i < 52 * 52;i ++){
        this->card_player_index[i] = vector<int>(this->player_number,-1);
    }

    // 用一个二维数组记录每个Private Combo的对应index,方便从一方的手牌找对方的同名卡牌的index
    for(int player_id = 0;player_id < player_number;player_id ++){
        vector<PrivateCards> privateCombos = private_cards[player_id];
        for(std::size_t i = 0;i < privateCombos.size();i ++){
            PrivateCards one_private_combo = privateCombos[i];
            this->card_player_index[one_private_combo.hashCode()][player_id] = i;
        }
    }

    this->initialboard = initialboard;
    this->setRelativeProbs();
}

vector<PrivateCards>& PrivateCardsManager::getPreflopCards(int player) {
    return this->private_cards[player];
}

int PrivateCardsManager::indPlayer2Player(int from_player, int to_player, int index) {
    if(index < 0 || index >= this->getPreflopCards(from_player).size()) throw runtime_error("index out of range");
    //PrivateCards player_combo = this->getPreflopCards(from_player)[index];
    int to_player_index = this->card_player_index[this->private_cards[from_player][index].hashCode()][to_player];
    if(to_player_index == -1){
        return -1;
    }else {
        return to_player_index;
    }
}

vector<float> PrivateCardsManager::getInitialReachProb(int player, uint64_t initialboard) {
    int cards_len =  this->private_cards[player].size();
    vector<float> probs = vector<float>(cards_len);
    for(int i = 0;i < cards_len;i ++){
        PrivateCards pc = this->private_cards[player][i];
        if(Card::boardsHasIntercept(initialboard,Card::boardInts2long(pc.get_hands()))) {
            probs[i] = 0;
        }else{
            probs[i] = this->private_cards[player][i].weight;
        }
    }
    return probs;
}

void PrivateCardsManager::setRelativeProbs() {
    int players = this->private_cards.size();
    for(int player_id = 0; player_id < players;player_id ++){
        // TODO 这里只考虑了两个玩家的情况
        int oppo = 1 - player_id;
        float player_prob_sum = 0;

        for(std::size_t i = 0;i < this->private_cards[player_id].size();i ++) {
            float oppo_prob_sum = 0;
            PrivateCards* player_card = &this->private_cards[player_id][i];
            uint64_t player_long = Card::boardInts2long(player_card->get_hands());

            //
            if (Card::boardsHasIntercept(player_long,this->initialboard)){
                continue;
            }

            for (auto oppo_card : this->private_cards[oppo]) {
                uint64_t oppo_long = Card::boardInts2long(oppo_card.get_hands());
                if (Card::boardsHasIntercept(oppo_long,this->initialboard)
                    || Card::boardsHasIntercept(oppo_long,player_long)
                        ){
                    continue;
                }
                oppo_prob_sum += oppo_card.weight;

            }
            player_card->relative_prob = oppo_prob_sum * player_card->weight;
            player_prob_sum += player_card->relative_prob;
        }
        for(std::size_t i = 0;i < this->private_cards[player_id].size();i ++) {
            this->private_cards[player_id][i].relative_prob = this->private_cards[player_id][i].relative_prob / player_prob_sum;
            //player_card.relative_prob = player_card.relative_prob / player_prob_sum;
        }

    }
}
