//
// Created by icybee&freezer on 2021/6/1.
//
#include "tools/Rule.h"
StreetSetting::StreetSetting(vector<float> bet_sizes, vector<float> raise_sizes, vector<float> donk_sizes,
                             bool allin) {
    this->bet_sizes = bet_sizes;
    this->raise_sizes = raise_sizes;
    this->donk_sizes = donk_sizes;
    this->allin = allin;
}

GameTreeBuildingSettings::GameTreeBuildingSettings(
            StreetSetting flop_ip,
            StreetSetting turn_ip,
            StreetSetting river_ip,
            StreetSetting flop_oop,
            StreetSetting turn_oop,
            StreetSetting river_oop):flop_ip(flop_ip),turn_ip(turn_ip),river_ip(river_ip),flop_oop(flop_oop),turn_oop(turn_oop),river_oop(river_oop){
}

StreetSetting & GameTreeBuildingSettings::getSettings(ActionNode::GameRound round, int player){
    if(!(player == 0 || player == 1)) throw runtime_error(fmt::format("player %s not known",player));
    if(round == GameTreeNode::GameRound::RIVER && player == 0) return this->river_ip;
    else if(round == GameTreeNode::GameRound::TURN && player == 0) return this->turn_ip;
    else if(round == GameTreeNode::GameRound::FLOP && player == 0) return this->flop_ip;
    else if(round == GameTreeNode::GameRound::RIVER && player == 1) return this->river_oop;
    else if(round == GameTreeNode::GameRound::TURN && player == 1) return this->turn_oop;
    else if(round == GameTreeNode::GameRound::FLOP && player == 1) return this->flop_oop;
    else throw new runtime_error(fmt::format("player %s and round not known",player));
}

Rule::Rule(Deck deck, float oop_commit, float ip_commit, int current_round, int raise_limit, float small_blind,
           float big_blind, float stack, GameTreeBuildingSettings build_settings):
           deck(deck),oop_commit(oop_commit),ip_commit(ip_commit),current_round(current_round),raise_limit(raise_limit),
           small_blind(small_blind),big_blind(big_blind),stack(stack),build_settings(build_settings){

}

float Rule::get_pot() {
    return this->oop_commit + this->ip_commit;
}

float Rule::get_commit(int player) {
    if (player == 0) return this->ip_commit;
    else if(player == 1) return this->oop_commit;
    else throw runtime_error("unknown player");
}
