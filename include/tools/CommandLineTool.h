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
    CommandLineTool();
    void startWorking(PokerSolver *ps);
    void execFromFile(const char *input_file, PokerSolver *ps);
    void processCommand(string &input, PokerSolver *ps);
    void dump_setting(const char *file);
    void set_pot(float val) {
        ip_commit = oop_commit = val / 2;
    }
    float get_pot() {
        return ip_commit + oop_commit;
    }
    void set_effective_stack(float val) {
        stack = val + ip_commit;
    }
    float get_effective_stack() {
        return stack - ip_commit;
    }
    bool set_board(string &str);
    bool set_bet_sizes(string &str, char delimiter = ',', vector<float> *sizes = nullptr);
    void build_tree(PokerSolver *ps) {
        if(!ps) return;
        ps->build_game_tree(oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,gtbs,allin_threshold);
    }
    void start_solve(PokerSolver *ps) {
        if(!ps) return;
        // cout << "<<<START SOLVING>>>" << endl;
        logger->log("<<<START SOLVING>>>");
        ps->train(
            range_ip,
            range_oop,
            board,
            // "tmp_log.txt",
            max_iteration,
            print_interval,
            algorithm,
            -1,
            accuracy,
            use_isomorphism,
            use_halffloats,
            thread_num,
            slice_cfr
        );
    }
// private:
    // enum Mode{
    //     HOLDEM,
    //     SHORTDECK
    // };
    // Mode mode;
    // string resource_dir;
    // PokerSolver ps;
    float oop_commit=5;
    float ip_commit=5;
    int current_round=1;
    int raise_limit=4;
    int thread_num=1;
    float small_blind=0.5;
    float big_blind=1;
    float stack=20 + 5;
    float allin_threshold = 0.67;
    string range_ip;
    string range_oop;
    string board;
    string res_file;
    string algorithm = "discounted_cfr";
    float accuracy = 0.1;
    int max_iteration=100;
    bool use_isomorphism=0;
    int use_halffloats=0;
    int print_interval=10;
    int slice_cfr = 0;
    int dump_rounds = 1;
    GameTreeBuildingSettings gtbs;
    Logger *logger = nullptr;
};

void split(const string& s, char delimiter, vector<string>& v);
void join(const vector<float> &vec, char delimiter, string &out);

template<class T>
string tostring(T val);
template<class T>
string tostring_oss(T val);

int cmd_api(string &input_file, string &resource_dir, string &mode, string &log_file);

#endif //BINDSOLVER_COMMANDLINETOOL_H
