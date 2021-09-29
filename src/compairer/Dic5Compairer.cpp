//
// Created by Xuefeng Huang on 2020/1/29.
//

#include "include/compairer/Dic5Compairer.h"

#include <utility>
#include <QFile>
#include <QTextStream>


Dic5Compairer::Dic5Compairer(string dic_dir,int lines):Compairer(std::move(dic_dir),lines){
    QFile infile(QString::fromStdString(this->dic_dir));
    if (!infile.open(QIODevice::ReadOnly)){
        throw runtime_error("unable to load compairer file");
    }
    QTextStream in(&infile);
    //progressbar bar(lines / 1000);
    int i = 0;
    //while (std::getline(infile, line))
    while (!in.atEnd())
    {
        string line = in.readLine().toStdString();
        vector<string> linesp = string_split(line,',');
        if(linesp.size() != 2){
           throw runtime_error(tfm::format("linesp not correct: %s",line));
        }
        string cards_str = linesp[0];
        int rank = stoi(linesp[1]);
        vector<string> cards = string_split(cards_str,'-');

        if(cards.size() != 5)
            throw runtime_error(
                    tfm::format(
                            "cards not correct: %s length %s",cards_str,cards.size()));

        //set<string> cards_set;
        //for(string one_card:cards) cards_set.insert(one_card);
        //this->card2rank[cards_set] = rank;

        if(this->cardslong2rank.find(Card::boardCards2long(cards)) != this->cardslong2rank.end()){
            throw runtime_error(
                    tfm::format("key repeated: %s",cards_str)
                    );
        }

        this->cardslong2rank[Card::boardCards2long(cards)] = rank;

        i ++;
        if(i % 1000 == 0) {
            //bar.update();
        }
    }
    //cout << endl;
}

Compairer::CompairResult Dic5Compairer::compairRanks(int rank_former, int rank_latter) {
    if (rank_former < rank_latter) {
        // rank更小的牌更大，0是同花顺
        return CompairResult::LARGER;
    } else if (rank_former > rank_latter) {
        return CompairResult::SMALLER;
    } else {
        // rank_former == rank_latter
        return CompairResult::EQUAL;
    }
}

Compairer::CompairResult
Dic5Compairer::compair(vector<Card> private_former, vector<Card> private_latter, vector<Card> public_board) {
    if(private_former.size() != 2)
        throw runtime_error(
                tfm::format("private former size incorrect,excepted 2, actually %s",private_former.size())
                );
    if(private_latter.size() != 2)
        throw runtime_error(
                tfm::format("private latter size incorrect,excepted 2, actually %s",private_latter.size())
        );
    if(public_board.size() != 5)
        throw runtime_error(
                tfm::format("public board size incorrect,excepted 2, actually %s",public_board.size())
        );

    vector<Card> former_cards(private_former);
    former_cards.insert(former_cards.end(),public_board.begin(),public_board.end());
    int rank_former = this->getRank(former_cards);

    vector<Card> latter_cards(private_latter);
    latter_cards.insert(latter_cards.end(),public_board.begin(),public_board.end());
    int rank_latter = this->getRank(latter_cards);

    return this->compairRanks(rank_former,rank_latter);
}

Compairer::CompairResult
Dic5Compairer::compair(vector<int> private_former, vector<int> private_latter, vector<int> public_board) {
    if(private_former.size() != 2)
        throw runtime_error(
                tfm::format("private former size incorrect,excepted 2, actually %s",private_former.size())
        );
    if(private_latter.size() != 2)
        throw runtime_error(
                tfm::format("private latter size incorrect,excepted 2, actually %s",private_latter.size())
        );
    if(public_board.size() != 5)
        throw runtime_error(
                tfm::format("public board size incorrect,excepted 2, actually %s",public_board.size())
        );

    vector<int> former_cards(private_former);
    former_cards.insert(former_cards.end(),public_board.begin(),public_board.end());
    int rank_former = this->getRank(former_cards);

    vector<int> latter_cards(private_latter);
    latter_cards.insert(latter_cards.end(),public_board.begin(),public_board.end());
    int rank_latter = this->getRank(latter_cards);

    return this->compairRanks(rank_former,rank_latter);
}

int Dic5Compairer::getRank(vector<Card> cards) {
    vector<int> cards_int(cards.size());
    for(int i = 0;i < cards.size();i++){
        cards_int[i] = cards[i].getCardInt();
    }
    return this->getRank(cards_int);
}

int Dic5Compairer::getRank(vector<int> cards) {
    Combinations<int> comb_cards(std::move(cards),5);
    int min_rank = numeric_limits<int>::max();
    for (const vector<int>& one_comb : comb_cards)
    {
        if(one_comb.size() != 5)throw runtime_error(tfm::format("card size incorrect: %s should be 5",one_comb.size()));
        uint64_t comb_uint64 = Card::boardInts2long(one_comb);
        int rank = this->cardslong2rank[comb_uint64];
        min_rank = min(rank,min_rank);
    }
    return min_rank;
}

int Dic5Compairer::get_rank(vector<Card> private_hand, vector<Card> public_board) {
    vector<Card> concatenate_vec(private_hand);
    concatenate_vec.insert(concatenate_vec.end(),public_board.begin(),public_board.end());
    return this->getRank(concatenate_vec);
}

int Dic5Compairer::get_rank(vector<int> private_hand, vector<int> public_board) {
    vector<int> concatenate_vec(private_hand);
    concatenate_vec.insert(concatenate_vec.end(),public_board.begin(),public_board.end());
    return this->getRank(concatenate_vec);
}

int Dic5Compairer::get_rank(uint64_t private_hand, uint64_t public_board) {
    return this->get_rank(Card::long2board(private_hand),Card::long2board(public_board));
}

