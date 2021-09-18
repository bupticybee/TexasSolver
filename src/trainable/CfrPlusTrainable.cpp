//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/trainable/CfrPlusTrainable.h"

CfrPlusTrainable::CfrPlusTrainable() {

}

CfrPlusTrainable::CfrPlusTrainable(shared_ptr<ActionNode> action_node, vector<PrivateCards> privateCards) {
    this->action_node = action_node;
    this->privateCards = privateCards;
    this->action_number = action_node->getChildrens().size();
    this->card_number = privateCards.size();

    this->r_plus = vector<float>(this->action_number * this->card_number);
    this->r_plus_sum = vector<float>(this->card_number);

    this->cum_r_plus = vector<float>(this->action_number * this->card_number);
    this->cum_r_plus_sum = vector<float>(this->card_number);
    this->retval = vector<float>(this->action_number * this->card_number);
}

bool CfrPlusTrainable::isAllZeros(vector<float> input_array) {
    for(float i:input_array){
        if (i != 0)return false;
    }
    return true;
}

const vector<float> CfrPlusTrainable::getAverageStrategy() {
    /*
    vector<float> retval(this->action_number * this->card_number);
    if(this->cum_r_plus_sum.empty() || this->isAllZeros(this->cum_r_plus_sum)){
        fill(retval.begin(),retval.end(),1.0 / this->action_number);
    }else {
        for (int action_id = 0; action_id < action_number; action_id++) {
            for (int private_id = 0; private_id < this->card_number; private_id++) {
                int index = action_id * this->card_number + private_id;
                if(this->cum_r_plus_sum[private_id] != 0) {
                    retval[index] = this->cum_r_plus[index] / this->cum_r_plus_sum[private_id];
                }else{
                    retval[index] = 1.0 / this->action_number;
                }
            }
        }
    }
    */
    return this->getcurrentStrategy();
}

const vector<float> CfrPlusTrainable::getcurrentStrategy() {
    if(this->r_plus_sum.empty()){
        fill(retval.begin(),retval.end(),1.0 / this->action_number);
    }else {
        for (int action_id = 0; action_id < action_number; action_id++) {
            for (int private_id = 0; private_id < this->card_number; private_id++) {
                int index = action_id * this->card_number + private_id;
                if(this->r_plus_sum[private_id] != 0) {
                    retval[index] = this->r_plus[index] / this->r_plus_sum[private_id];
                }else{
                    retval[index] = 1.0 / this->action_number;
                }
                if(this->r_plus[index] != this->r_plus[index]) throw runtime_error("nan found");
                /*
                if(this.r_plus_sum[private_id] == 0)
                {
                    System.out.println("Exception regret status, r_plus_sum == 0:");
                    System.out.println(String.format("r plus length %s , card num %s",r_plus.length,this.card_number));
                    for(int i = index % this.card_number;i < this.r_plus.length;i += this.card_number){
                        System.out.print(String.format("%s:%s ",i,this.r_plus[i]));
                        if(i == index){
                            System.out.print("[current]");
                        }
                    }
                    System.out.println();
                    System.out.println();
                    throw new RuntimeException();
                }
                 */
            }
        }
    }
    return retval;
}

void CfrPlusTrainable::updateRegrets(const vector<float>& regrets, int iteration_number, const vector<float>& reach_probs) {
    this->regrets = regrets;
    if(regrets.size() != this->action_number * this->card_number) throw runtime_error("length not match");

    //Arrays.fill(this.r_plus_sum,0);
    fill(r_plus_sum.begin(),r_plus_sum.end(),0);
    fill(cum_r_plus_sum.begin(),cum_r_plus_sum.end(),0);
    for (int action_id = 0;action_id < action_number;action_id ++) {
        for(int private_id = 0;private_id < this->card_number;private_id ++){
            int index = action_id * this->card_number + private_id;
            float one_reg = regrets[index];

            // 更新 R+
            this->r_plus[index] = max((float)0.0,one_reg + this->r_plus[index]);
            this->r_plus_sum[private_id] += this->r_plus[index];

            // 更新累计策略
            this->cum_r_plus[index] += this->r_plus[index] * iteration_number;
            this->cum_r_plus_sum[private_id] += this->cum_r_plus[index];
        }
    }
}

json CfrPlusTrainable::dump_strategy(bool with_state) {
    if(with_state) throw runtime_error("state storage not implemented");

    json strategy;
    vector<float> average_strategy = this->getcurrentStrategy();
    vector<GameActions> game_actions = action_node->getActions();
    vector<string> actions_str;
    for(GameActions one_action:game_actions) actions_str.push_back(
                one_action.toString()
        );

    //SolverEnvironment se = SolverEnvironment.getInstance();
    //Compairer comp = se.getCompairer();

    for(int i = 0;i < this->privateCards.size();i ++){
        PrivateCards one_private_card = this->privateCards[i];
        vector<float> one_strategy(this->action_number);

        /*
        int[] initialBoard = new int[]{
                Card.strCard2int("Kd"),
                Card.strCard2int("Jd"),
                Card.strCard2int("Td"),
                Card.strCard2int("7s"),
                Card.strCard2int("8s")
        };
        int rank = comp.get_rank(new int[]{one_private_card.card1,one_private_card.card2},initialBoard);
         */

        for(int j = 0;j < this->action_number;j ++){
            int strategy_index = j * this->privateCards.size() + i;
            one_strategy[j] = average_strategy[strategy_index];
        }
        strategy[tfm::format("%s",one_private_card.toString())] = one_strategy;
    }

    json retjson;
    retjson["actions"] = actions_str;
    retjson["strategy"] = strategy;
    return retjson;
}

Trainable::TrainableType CfrPlusTrainable::get_type() {
    return CFR_PLUS_TRAINABLE;
}
