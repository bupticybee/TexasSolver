//
// Created by icybee&freezer on 2021/6/1.
//
#include "include/tools/Rule.h"

Rule::Rule(Deck deck, float oop_commit, float ip_commit, int current_round, int raise_limit, float small_blind,
           float big_blind, float stack, GameTreeBuildingSettings build_settings,float allin_threshold):
           deck(deck),oop_commit(oop_commit),ip_commit(ip_commit),current_round(current_round),raise_limit(raise_limit),
           small_blind(small_blind),big_blind(big_blind),stack(stack),build_settings(build_settings),allin_threshold(allin_threshold){
    if(oop_commit != ip_commit) throw runtime_error("ip commit should equal with oop commit");
    this->initial_effective_stack = stack;
}

float Rule::get_pot() {
    return this->oop_commit + this->ip_commit;
}

float Rule::get_commit(int player) {
    if (player == 0) return this->ip_commit;
    else if(player == 1) return this->oop_commit;
    else throw runtime_error("unknown player");
}
