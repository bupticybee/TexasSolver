//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_COMPAIRER_H
#define TEXASSOLVER_COMPAIRER_H
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <include/Card.h>
#include "include/tools/tinyformat.h"

class Compairer {
public:
    enum CompairResult{LARGER,EQUAL,SMALLER};
    Compairer()= default;;
    Compairer(string dic_dir,int lines){
        this->dic_dir = std::move(dic_dir);
        this->lines = lines;
    };
    virtual CompairResult compair(vector<Card> private_former, vector<Card> private_latter, vector<Card> public_board) = 0;
    virtual CompairResult compair(vector<int> private_former, vector<int> private_latter, vector<int> public_board) = 0;
    virtual int get_rank(vector<Card> private_hand,vector<Card> public_board) = 0;
    virtual int get_rank(vector<int> private_hand,vector<int> public_board) = 0;
    virtual int get_rank(uint64_t private_hand,uint64_t public_board) = 0;

protected:
    string dic_dir;
    int lines{};
};


#endif //TEXASSOLVER_COMPAIRER_H
