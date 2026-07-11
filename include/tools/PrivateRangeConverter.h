//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PRIVATERANGECONVERTER_H
#define TEXASSOLVER_PRIVATERANGECONVERTER_H


#include <ranges/PrivateCards.h>
#include "library.h"
using namespace std;

class PrivateRangeConverter {
public:
    // has_exact_combo, if non-null, is set to true when range_str contains at
    // least one exact suited combo token (e.g. "KhQh:1.0") rather than a
    // rank-class token ("AA", "AKs", "AKo"). Callers use this to detect
    // ranges that are not suit-symmetric, since suit isomorphism assumes
    // every rank-class expands identically across all 4 suits.
    static vector<PrivateCards> rangeStr2Cards(string range_str,vector<int> initial_boards,bool* has_exact_combo=nullptr);

};


#endif //TEXASSOLVER_PRIVATERANGECONVERTER_H
