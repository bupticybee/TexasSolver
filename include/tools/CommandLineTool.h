//
// Created by bytedance on 7.6.21.
//

#ifndef BINDSOLVER_COMMANDLINETOOL_H
#define BINDSOLVER_COMMANDLINETOOL_H
#include <string>
#include <vector>
#include <iostream>
#include "runtime/PokerSolver.h"

using namespace std;
class CommandLineTool{
public:
    CommandLineTool();
    void startWorking();
    void processCommand(string input);
private:
    PokerSolver ps;
    float oop_commit=5;
    float ip_commit=5;
    int current_round=1;
    int raise_limit=4;
    float small_blind=0.5;
    float big_blind=1;
    float stack=20 + 5;
    float allin_threshold = 0.67;
    string range_ip;
    string range_oop;
    string board;
    float accuracy;
    shared_ptr<GameTreeBuildingSettings> gtbs;
};

#endif //BINDSOLVER_COMMANDLINETOOL_H
