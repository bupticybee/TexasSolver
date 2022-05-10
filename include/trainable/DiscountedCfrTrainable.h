//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_DISCOUNTEDCFRTRAINABLE_H
#define TEXASSOLVER_DISCOUNTEDCFRTRAINABLE_H
#include <include/tools/half-1-12-0.h>
#include <include/nodes/ActionNode.h>
#include <include/ranges/PrivateCards.h>
#include "Trainable.h"

using namespace std;

class DiscountedCfrTrainable:public Trainable {
private:
    // typedef float storage;
    typedef half_float::half storage;
    ActionNode& action_node;
    vector<PrivateCards>* privateCards;
    int action_number;
    int card_number;
    vector<storage> r_plus;
    vector<storage> evs;
    constexpr static float alpha = 1.5f;
    constexpr static float beta = 0.5f;
    constexpr static float gamma = 2;
    constexpr static float theta = 0.9f;
    // TODO 這裏能不能减肥
    vector<storage> r_plus_sum;
    vector<storage> cum_r_plus;
    //vector<float> cum_r_plus_sum;
    //vector<float> current_strategy;
    //vector<float> average_strategy;
public:
    DiscountedCfrTrainable(vector<PrivateCards> *privateCards,
                           ActionNode &actionNode);
    bool isAllZeros(const vector<float>& input_array);

    const vector<float> getAverageStrategy() override;

    const vector<float> getcurrentStrategy() override;

    void updateRegrets(const vector<float>& regrets, int iteration_number, const vector<float>& reach_probs) override;

    void setEv(const vector<float>& evs) override;

    void copyStrategy(shared_ptr<Trainable> other_trainable);

    json dump_strategy(bool with_state) override;

    json dump_evs() override;

private:

    const vector<float> getcurrentStrategyNoCache();

    TrainableType get_type() override;

};


#endif //TEXASSOLVER_DISCOUNTEDCFRTRAINABLE_H
