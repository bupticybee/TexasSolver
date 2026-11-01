//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "trainable/DiscountedCfrTrainable.h"
//#define DEBUG;

DiscountedCfrTrainable::DiscountedCfrTrainable(vector<PrivateCards> *privateCards,
                                               ActionNode &actionNode) : action_node(actionNode) {
    this->privateCards = privateCards;
    this->action_number = action_node.getChildrens().size();
    this->card_number = privateCards->size();

    this->r_plus = vector<float>(this->action_number * this->card_number);
    this->r_plus_sum = vector<float>(this->card_number);

    this->cum_r_plus = vector<float>(this->action_number * this->card_number);
    //this->cum_r_plus_sum = vector<float>(this->card_number);
}

bool DiscountedCfrTrainable::isAllZeros(const vector<float>& input_array) {
    for(float i:input_array){
        if (i != 0)return false;
    }
    return true;
}

const vector<float> DiscountedCfrTrainable::getAverageStrategy() {
    vector<float> average_strategy;
    average_strategy = vector<float>(this->action_number * this->card_number);
    for (int private_id = 0; private_id < this->card_number; private_id++) {
        float r_plus_sum = 0;
        for (int action_id = 0; action_id < action_number; action_id++) {
            int index = action_id * this->card_number + private_id;
            r_plus_sum += this->cum_r_plus[index];
        }

        for (int action_id = 0; action_id < action_number; action_id++) {
            int index = action_id * this->card_number + private_id;
            if(r_plus_sum) {
                average_strategy[index] = this->cum_r_plus[index] / r_plus_sum;
            }else{
                average_strategy[index] = 1.0 / this->action_number;
            }
        }
    }
    return average_strategy;
}

const vector<float> DiscountedCfrTrainable::getcurrentStrategy() {
    return this->getcurrentStrategyNoCache();
}

void DiscountedCfrTrainable::copyStrategy(shared_ptr<Trainable> other_trainable){
    shared_ptr<DiscountedCfrTrainable> trainable = dynamic_pointer_cast<DiscountedCfrTrainable>(other_trainable);
    this->r_plus.assign(trainable->r_plus.begin(),trainable->r_plus.end());
    this->cum_r_plus.assign(trainable->cum_r_plus.begin(),trainable->cum_r_plus.end());
}

const vector<float> DiscountedCfrTrainable::getcurrentStrategyNoCache() {
    vector<float> current_strategy;
    current_strategy = vector<float>(this->action_number * this->card_number);
    if(this->r_plus_sum.empty()){
        fill(current_strategy.begin(),current_strategy.end(),1.0 / this->action_number);
    }else {
        const int cards = this->card_number;
        const float uniform = 1.0 / this->action_number;
        const float* __restrict sum = this->r_plus_sum.data();
        for (int action_id = 0; action_id < action_number; action_id++) {
            const float* __restrict regret = this->r_plus.data() + action_id * cards;
            float* __restrict out = current_strategy.data() + action_id * cards;
            for (int private_id = 0; private_id < cards; private_id++) {
                const float positive = regret[private_id] > 0 ? regret[private_id] : 0;
                const float share = positive / sum[private_id];
                out[private_id] = sum[private_id] != 0 ? share : uniform;
            }
        }
    }
    return current_strategy;
}

void DiscountedCfrTrainable::updateRegrets(const vector<float>& regrets, int iteration_number, const vector<float>& reach_probs) {
    auto alpha_power = pow(iteration_number, this->alpha);
    const double alpha_coef = alpha_power / (1 + alpha_power);
    const float beta_coef = this->beta;
    const int cards = this->card_number;

    fill(r_plus_sum.begin(),r_plus_sum.end(),0);
    float* __restrict sum = this->r_plus_sum.data();
    for (int action_id = 0;action_id < action_number;action_id ++) {
        const float* __restrict regret = regrets.data() + action_id * cards;
        float* __restrict r_plus_action = this->r_plus.data() + action_id * cards;
        for(int private_id = 0;private_id < cards;private_id ++){
            // 更新 R+
            const float updated = regret[private_id] + r_plus_action[private_id];
            const float discounted = updated > 0 ? float(updated * alpha_coef) : updated * beta_coef;
            r_plus_action[private_id] = discounted;
            sum[private_id] += discounted > 0 ? discounted : 0;
        }
    }
    vector<float> current_strategy = this->getcurrentStrategyNoCache();
    const float strategy_coef = pow(((float)iteration_number / (iteration_number + 1)),gamma);
    const float theta_coef = this->theta;
    const int total = this->action_number * cards;
    const float* __restrict strategy = current_strategy.data();
    float* __restrict cumulative = this->cum_r_plus.data();
    for (int index = 0;index < total;index ++) {
        const float decayed = cumulative[index] * theta_coef;
        cumulative[index] = decayed + strategy[index] * strategy_coef;
    }
}

json DiscountedCfrTrainable::dump_strategy(bool with_state) {
    if(with_state) throw runtime_error("state storage not implemented");

    json strategy;
    const vector<float>& average_strategy = this->getAverageStrategy();
    vector<GameActions>& game_actions = action_node.getActions();
    vector<string> actions_str;
    for(GameActions& one_action:game_actions) {
        actions_str.push_back(
                one_action.toString()
        );
    }

    for(int i = 0;i < this->privateCards->size();i ++){
        PrivateCards& one_private_card = (*this->privateCards)[i];
        vector<float> one_strategy(this->action_number);

        for(int j = 0;j < this->action_number;j ++){
            int strategy_index = j * this->privateCards->size() + i;
            one_strategy[j] = average_strategy[strategy_index];
        }
        strategy[fmt::format("{}",one_private_card.toString())] = one_strategy;
    }

    json retjson;
    retjson["actions"] = std::move(actions_str);
    retjson["strategy"] = std::move(strategy);
    return std::move(retjson);
}

Trainable::TrainableType DiscountedCfrTrainable::get_type() {
    return DISCOUNTED_CFR_TRAINABLE;
}
