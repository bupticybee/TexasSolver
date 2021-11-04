//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PRIVATERANGECONVERTER_H
#define TEXASSOLVER_PRIVATERANGECONVERTER_H


#include <include/ranges/PrivateCards.h>
#include "include/library.h"
using namespace std;

class PrivateRangeConverter {
public:
    static vector<PrivateCards> rangeStr2Cards(string range_str,vector<int> initial_boards);

};


#endif //TEXASSOLVER_PRIVATERANGECONVERTER_H
