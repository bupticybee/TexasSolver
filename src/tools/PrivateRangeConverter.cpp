//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/tools/PrivateRangeConverter.h"

vector<PrivateCards> PrivateRangeConverter::rangeStr2Cards(string range_str, vector<int> initial_boards) {
    vector<string> range_list = string_split(range_str,',');
    vector<PrivateCards> private_cards;

    for(string one_range:range_list){
        PrivateCards this_card;
        vector<string> cardstr_arr = string_split(one_range,':');
        if (cardstr_arr.size() > 2 || cardstr_arr.empty()){
            throw runtime_error("':' number exceeded 2");
        }
        float weight = 1;

        one_range = cardstr_arr[0];
        if(cardstr_arr.size() == 2){
            weight = atof(cardstr_arr[1].c_str());
        }
        if(weight <= 0.005){
            continue;
        }

        int range_len = one_range.length();
        // TODO finish here , convert str to a PrivateRanges[]
        if(range_len == 3){
            if(one_range.at(2) == 's'){
                char rank1 = one_range.at(0);
                char rank2 = one_range.at(1);
                if(rank1 == rank2) throw runtime_error(tfm::format("%s%ss is not a valid card desc",rank1,rank2));
                for(const string& one_suit :Card::getSuits()){
                    int card1 = Card::strCard2int(rank1 + one_suit);
                    int card2 = Card::strCard2int(rank2 + one_suit);
                    this_card = PrivateCards(card1,card2,weight);
                    private_cards.push_back(this_card);
                }

            }else if(one_range.at(2) == 'o'){
                char rank1 = one_range.at(0);
                char rank2 = one_range.at(1);

                vector<string> suits = Card::getSuits();
                for(std::size_t i = 0;i < suits.size();i++){
                    string one_suit = suits[i];
                    int begin_index = rank1 == rank2 ? i:0;
                    for(std::size_t j = begin_index;j < suits.size();j++){
                        string another_suit = suits[j];
                        if(one_suit == another_suit){
                            continue;
                        }
                        int card1 = Card::strCard2int(rank1 + one_suit);
                        int card2 = Card::strCard2int(rank2 + another_suit);
                        if(Card::boardsHasIntercept(
                                Card::boardInts2long(vector<int>{card1,card2}),
                                Card::boardInts2long(initial_boards)
                        )){
                            continue;
                        }
                        this_card = PrivateCards(card1, card2, weight);
                        private_cards.push_back(this_card);
                    }
                }
            }else{
                throw runtime_error("format not recognize");
            }
        }else if(range_len == 2){
            char rank1 = one_range.at(0);
            char rank2 = one_range.at(1);
            vector<string> suits = Card::getSuits();
            for(std::size_t i = 0;i < suits.size();i++){
                string one_suit = suits[i];
                int begin_index = rank1 == rank2 ? i:0;
                for(std::size_t j = begin_index;j < suits.size();j++){
                    string another_suit = suits[j];
                    if(one_suit == another_suit && rank1 == rank2){
                        continue;
                    }
                    int card1 = Card::strCard2int(rank1 + one_suit);
                    int card2 = Card::strCard2int(rank2 + another_suit);
                    if(Card::boardsHasIntercept(
                            Card::boardInts2long(vector<int>{card1,card2}),
                            Card::boardInts2long(initial_boards)
                    )){
                        continue;
                    }
                    this_card = PrivateCards(card1, card2, weight);
                    private_cards.push_back(this_card);
                }
            }

        }else throw runtime_error(tfm::format(" range str %s len not valid ",one_range));
    }

    // 排除初试range中重复的情况
    for(std::size_t i = 0;i < private_cards.size();i ++){
        for(std::size_t j = i + 1;j < private_cards.size();j ++) {
            PrivateCards one_cards = private_cards[i];
            PrivateCards another_cards = private_cards[j];
            if (one_cards.card1 == another_cards.card1 && one_cards.card2 == another_cards.card2){
                throw runtime_error(tfm::format("card %s %s duplicate"
                        , Card::intCard2Str(one_cards.card1)
                        , Card::intCard2Str(one_cards.card2)
                ));
            }
            if(one_cards.card1 == another_cards.card2 && one_cards.card2 == another_cards.card1) {
                throw runtime_error(tfm::format("card %s %s duplicate"
                        , Card::intCard2Str(one_cards.card1)
                        , Card::intCard2Str(one_cards.card2)
                ));
            }
        }
    }

    vector<PrivateCards> private_cards_list(private_cards.size());
    for(std::size_t i = 0;i < private_cards.size();i ++){
        private_cards_list[i] = private_cards[i];
        //System.out.print(String.format("[%s-%s]",Card.intCard2Str(private_cards_list[i].card1),Card.intCard2Str(private_cards_list[i].card2)));
    }
    /*
        output all private combos

    System.out.println("private range number:");
    System.out.println(private_cards.size());
    System.out.println();
     */
    return private_cards_list;

}
