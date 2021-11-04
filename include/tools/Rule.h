//
// Created by icybee&freezer on 2021/6/1.
//

#ifndef BINDSOLVER_RULE_H
#define BINDSOLVER_RULE_H

#include "include/Deck.h"
#include "include/tools/StreetSetting.h"
#include "include/tools/GameTreeBuildingSettings.h"

using namespace std;

class Rule{
public:
    Deck& deck;
    float oop_commit;
    float ip_commit;
    int current_round; // 0-preflop 1-flop 2-turn 3-river
    int raise_limit;
    float small_blind;
    float big_blind;
    float stack;
    GameTreeBuildingSettings build_settings;
    vector<int> players = {0,1};
    float allin_threshold;
    float initial_effective_stack;
    Rule(
            Deck deck,
            float oop_commit,
            float ip_commit,
            int current_round,
            int raise_limit,
            float small_blind,
            float big_blind,
            float stack,
            GameTreeBuildingSettings build_settings,
            float allin_threshold
    ) ;

    float get_pot();
    float get_commit(int player);
};


#endif //BINDSOLVER_RULE_H
