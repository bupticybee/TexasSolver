//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_DIC5COMPAIRER_H
#define TEXASSOLVER_DIC5COMPAIRER_H

#include <map>
#include <set>
#include <fstream>
#include <unordered_map>
#include "include/compairer/Compairer.h"
#include "include/library.h"
#include "include/tools/progressbar.h"

struct FiveCardsStrength {
    void convert(unordered_map<uint64_t, int>& strength_map);
    bool load(const char* file_path);
    bool save(const char* file_path);
    int operator[](uint64_t hash);
    bool check(unordered_map<uint64_t, int>& strength_map);
    unordered_map<uint64_t, int> flush_map, other_map;
};

class Dic5Compairer:public Compairer{

public:
    Dic5Compairer(string dic_dir,int lines, string dic_dir_bin);

    CompairResult compair(vector<Card> private_former, vector<Card> private_latter, vector<Card> public_board) override;

    CompairResult compair(vector<int> private_former, vector<int> private_latter, vector<int> public_board) override;

    int get_rank(vector<Card> private_hand, vector<Card> public_board) override;

    int get_rank(vector<int> private_hand, vector<int> public_board) override;

    int get_rank(uint64_t private_hand, uint64_t public_board) override;


private:
    //unordered_map<set<string>,int> card2rank;
    unordered_map<uint64_t,int> cardslong2rank;
    FiveCardsStrength fcs;
    int getRank(vector<Card> cards);
    int getRank(vector<int> cards);
    static CompairResult compairRanks(int rank_former,int rank_latter);

};


#endif //TEXASSOLVER_DIC5COMPAIRER_H
