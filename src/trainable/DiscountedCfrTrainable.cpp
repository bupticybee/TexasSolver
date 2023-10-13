//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/trainable/DiscountedCfrTrainable.h"
//#define DEBUG;

DiscountedCfrTrainable::DiscountedCfrTrainable(vector<PrivateCards> *privateCards,
                                               ActionNode &actionNode) : action_node(actionNode) {
    this->privateCards = privateCards;
    this->action_number = action_node.getChildrens().size();
    this->card_number = privateCards->size();

    this->evs = vector<float>(this->action_number * this->card_number,0.0);
    this->r_plus = vector<float>(this->action_number * this->card_number,0.0);
    this->r_plus_sum = vector<float>(this->card_number,0.0);

    this->cum_r_plus = vector<float>(this->action_number * this->card_number,0.0);
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
        for (int action_id = 0; action_id < action_number; action_id++) {
            for (int private_id = 0; private_id < this->card_number; private_id++) {
                int index = action_id * this->card_number + private_id;
                if(this->r_plus_sum[private_id] != 0) {
                    current_strategy[index] = max(float(0.0),this->r_plus[index]) / this->r_plus_sum[private_id];
                }else{
                    current_strategy[index] = 1.0 / (this->action_number);
                }
#ifdef DEBUG
                if(this->r_plus[index] != this->r_plus[index]) throw runtime_error("nan found");
#endif
            }
        }
    }
    return current_strategy;
}

void DiscountedCfrTrainable::setEv(const vector<float>& evs){
    if(evs.size() != this->evs.size()) throw runtime_error("size mismatch in discountcfrtrainable setEV");
    for(std::size_t i = 0;i < evs.size();i ++) if(evs[i] == evs[i])this->evs[i] = evs[i];
}

void DiscountedCfrTrainable::updateRegrets(const vector<float>& regrets, int iteration_number, const vector<float>& reach_probs) {

#ifdef DEBUG
    if(regrets.size() != this->action_number * this->card_number) throw runtime_error("length not match");
#endif

    auto alpha_coef = pow(iteration_number, this->alpha);
    alpha_coef = alpha_coef / (1 + alpha_coef);

    //Arrays.fill(this.r_plus_sum,0);
    fill(r_plus_sum.begin(),r_plus_sum.end(),0);
    //fill(cum_r_plus_sum.begin(),cum_r_plus_sum.end(),0);
    for (int action_id = 0;action_id < action_number;action_id ++) {
        for(int private_id = 0;private_id < this->card_number;private_id ++){
            int index = action_id * this->card_number + private_id;
            float one_reg = regrets[index];

            // 更新 R+
            this->r_plus[index] = one_reg + this->r_plus[index];
            if(this->r_plus[index] > 0){
                this->r_plus[index] *= alpha_coef;
            }else{
                this->r_plus[index] *= beta;
            }

            this->r_plus_sum[private_id] += max(float(0.0),this->r_plus[index]);

            // 更新累计策略
            // this.cum_r_plus[index] += this.r_plus[index] * iteration_number;
            // this.cum_r_plus_sum[private_id] += this.cum_r_plus[index];
        }
    }
    vector<float> current_strategy = this->getcurrentStrategyNoCache();
    float strategy_coef = pow(((float)iteration_number / (iteration_number + 1)),gamma);
    for (int action_id = 0;action_id < action_number;action_id ++) {
        for(int private_id = 0;private_id < this->card_number;private_id ++) {
            int index = action_id * this->card_number + private_id;
            this->cum_r_plus[index] *= this->theta;
            this->cum_r_plus[index] += current_strategy[index] * strategy_coef;// * reach_probs[private_id];
            //this->cum_r_plus_sum[private_id] += this->cum_r_plus[index] ;
        }
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

    for(std::size_t i = 0;i < this->privateCards->size();i ++){
        PrivateCards& one_private_card = (*this->privateCards)[i];
        vector<float> one_strategy(this->action_number);

        for(int j = 0;j < this->action_number;j ++){
            std::size_t strategy_index = j * this->privateCards->size() + i;
            one_strategy[j] = average_strategy[strategy_index];
        }
        strategy[tfm::format("%s",one_private_card.toString())] = one_strategy;
    }

    json retjson;
    retjson["actions"] = std::move(actions_str);
    retjson["strategy"] = std::move(strategy);
    return std::move(retjson);
}

json DiscountedCfrTrainable::dump_evs() {
    json evs;
    const vector<float>& average_evs = this->evs;
    vector<GameActions>& game_actions = action_node.getActions();
    vector<string> actions_str;
    for(GameActions& one_action:game_actions) {
        actions_str.push_back(
                one_action.toString()
        );
    }

    for(std::size_t i = 0;i < this->privateCards->size();i ++){
        PrivateCards& one_private_card = (*this->privateCards)[i];
        vector<float> one_evs(this->action_number);

        for(int j = 0;j < this->action_number;j ++){
            std::size_t evs_index = j * this->privateCards->size() + i;
            one_evs[j] = average_evs[evs_index];
        }
        evs[tfm::format("%s",one_private_card.toString())] = one_evs;
    }

    json retjson;
    retjson["actions"] = std::move(actions_str);
    retjson["evs"] = std::move(evs);
    return std::move(retjson);
}

Trainable::TrainableType DiscountedCfrTrainable::get_type() {
    return DISCOUNTED_CFR_TRAINABLE;
}
