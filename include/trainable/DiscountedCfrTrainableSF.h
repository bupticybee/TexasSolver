//
// Half float version created by Martin Ostermann on 2022-5-16,
// based DiscountableCfrTrainable.h from Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_DiscountedCfrTrainableSF_H
#define TEXASSOLVER_DiscountedCfrTrainableSF_H
#include <include/tools/half-1-12-0.h>
#include <include/nodes/ActionNode.h>
#include <include/ranges/PrivateCards.h>
#include "Trainable.h"


using namespace std;
using half_float::half;

class DiscountedCfrTrainableSF:public Trainable {
private:
    typedef half EvsStorage;
    typedef float RplusStorage;
    typedef float CumRplusStorage;
    ActionNode& action_node;
    vector<PrivateCards>* privateCards;
    int action_number;
    int card_number;
    vector<RplusStorage> r_plus;
    vector<EvsStorage> evs;
    constexpr static float alpha = 1.5f;
    constexpr static float beta = 0.5f;
    constexpr static float gamma = 2;
    constexpr static float theta = 0.9f;
    vector<CumRplusStorage> cum_r_plus;
public:
    DiscountedCfrTrainableSF(vector<PrivateCards> *privateCards,
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



#endif //TEXASSOLVER_DiscountedCfrTrainableSF_H
