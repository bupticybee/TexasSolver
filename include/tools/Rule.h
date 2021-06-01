//
// Created by icybee&freezer on 2021/6/1.
//

#ifndef BINDSOLVER_RULE_H
#define BINDSOLVER_RULE_H

#include "Deck.h"
#include "GameTree.h"
using namespace std;

class StreetSetting {
public:
    vector<float> bet_sizes;
    vector<float> raise_sizes;
    vector<float> donk_sizes;
    bool allin;

    StreetSetting(vector<float> bet_sizes, vector<float> raise_sizes, vector<float> donk_sizes, bool allin);

};

class GameTreeBuildingSettings {
public:
    StreetSetting flop_ip;
    StreetSetting turn_ip;
    StreetSetting river_ip;
    StreetSetting flop_oop;
    StreetSetting turn_oop;
    StreetSetting river_oop;
    GameTreeBuildingSettings(
            StreetSetting flop_ip,
            StreetSetting turn_ip,
            StreetSetting river_ip,
            StreetSetting flop_oop,
            StreetSetting turn_oop,
            StreetSetting river_oop) ;

    StreetSetting & getSettings(ActionNode::GameRound round, int player);
};

class Rule{
public:
    Deck& deck;
    float oop_commit;
    float ip_commit;
    int current_round;
    int raise_limit;
    float small_blind;
    float big_blind;
    float stack;
    GameTreeBuildingSettings build_settings;
    vector<int> players = {0,1};
    Rule(
            Deck deck,
            float oop_commit,
            float ip_commit,
            int current_round,
            int raise_limit,
            float small_blind,
            float big_blind,
            float stack,
            GameTreeBuildingSettings build_settings
    ) ;

    float get_pot();
    float get_commit(int player);
};


#endif //BINDSOLVER_RULE_H
