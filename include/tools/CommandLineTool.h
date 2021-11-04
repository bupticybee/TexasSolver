//
// Created by bytedance on 7.6.21.
//

#ifndef BINDSOLVER_COMMANDLINETOOL_H
#define BINDSOLVER_COMMANDLINETOOL_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "include/runtime/PokerSolver.h"

using namespace std;
class CommandLineTool{
public:
    CommandLineTool(string mode,string resource_dir);
    void startWorking();
    void execFromFile(string input_file);
    void processCommand(string input);
private:
    enum Mode{
        HOLDEM,
        SHORTDECK
    };
    Mode mode;
    string resource_dir;
    PokerSolver ps;
    float oop_commit=5;
    float ip_commit=5;
    int current_round=1;
    int raise_limit=4;
    int thread_number=1;
    float small_blind=0.5;
    float big_blind=1;
    float stack=20 + 5;
    float allin_threshold = 0.67;
    string range_ip;
    string range_oop;
    string board;
    float accuracy;
    int max_iteration=100;
    int use_isomorphism=0;
    int print_interval=10;
    int dump_rounds = 1;
    shared_ptr<GameTreeBuildingSettings> gtbs;
};

#endif //BINDSOLVER_COMMANDLINETOOL_H
