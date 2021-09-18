//
// Created by bytedance on 2.6.21.
//

#include "include/tools/StreetSetting.h"

StreetSetting::StreetSetting(vector<float> bet_sizes, vector<float> raise_sizes, vector<float> donk_sizes,
                             bool allin) {
    this->bet_sizes = bet_sizes;
    this->raise_sizes = raise_sizes;
    this->donk_sizes = donk_sizes;
    this->allin = allin;
}

