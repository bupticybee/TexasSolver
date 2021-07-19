//
// Created by bytedance on 2.6.21.
//

#ifndef BINDSOLVER_STREETSETTING_H
#define BINDSOLVER_STREETSETTING_H
#include <vector>
using namespace std;

class StreetSetting {
public:
    vector<float> bet_sizes;
    vector<float> raise_sizes;
    vector<float> donk_sizes;
    bool allin;

    StreetSetting(vector<float> bet_sizes, vector<float> raise_sizes, vector<float> donk_sizes, bool allin);

};

#endif //BINDSOLVER_STREETSETTING_H
