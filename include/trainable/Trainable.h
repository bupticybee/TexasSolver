//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_TRAINABLE_H
#define TEXASSOLVER_TRAINABLE_H
#include <vector>
#include "include/json.hpp"
using namespace std;
using json = nlohmann::json;

class Trainable {
public:
    enum TrainableType {
        CFR_PLUS_TRAINABLE,
        DISCOUNTED_CFR_TRAINABLE
    };
    virtual const vector<float> getAverageStrategy() = 0;
    virtual const vector<float> getcurrentStrategy() = 0;
    virtual void updateRegrets(const vector<float>& regrets,int iteration_number,const vector<float>& reach_probs) = 0;
    virtual void setEv(const vector<float>& evs) = 0;
    virtual void copyStrategy(shared_ptr<Trainable> other_trainable) = 0;
    virtual json dump_strategy(bool with_state) = 0;
    virtual json dump_evs() = 0;
    virtual TrainableType get_type() = 0;
};


#endif //TEXASSOLVER_TRAINABLE_H
