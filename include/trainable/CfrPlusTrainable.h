//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_CFRPLUSTRAINABLE_H
#define TEXASSOLVER_CFRPLUSTRAINABLE_H

#include <include/nodes/ActionNode.h>
#include <include/ranges/PrivateCards.h>
#include "include/trainable/Trainable.h"
using namespace std;

class CfrPlusTrainable : public Trainable{
private:
    shared_ptr<ActionNode> action_node;
    vector<PrivateCards> privateCards;
    int action_number;
    int card_number;
    vector<float> r_plus;
    vector<float> r_plus_sum;
    vector<float> cum_r_plus;
    vector<float> cum_r_plus_sum;
    vector<float> regrets;
    vector<float> retval;
public:
    CfrPlusTrainable();
    CfrPlusTrainable(shared_ptr<ActionNode> action_node, vector<PrivateCards> privateCards);
    bool isAllZeros(vector<float> input_array);

    const vector<float> getAverageStrategy() override;

    const vector<float> getcurrentStrategy() override;

    void updateRegrets(const vector<float>& regrets, int iteration_number, const vector<float>& reach_probs) override;

    json dump_strategy(bool with_state) override;

    TrainableType get_type() override;
};


#endif //TEXASSOLVER_CFRPLUSTRAINABLE_H
