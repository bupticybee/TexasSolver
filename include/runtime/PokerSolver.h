//
// Created by Xuefeng Huang on 2020/2/6.
//

#ifndef TEXASSOLVER_POKERSOLVER_H
#define TEXASSOLVER_POKERSOLVER_H

#include <string>
#include <vector>
#include "include/compairer/Dic5Compairer.h"
#include "include/tools/PrivateRangeConverter.h"
#include "include/solver/CfrSolver.h"
#include "include/solver/PCfrSolver.h"
#include "include/library.h"
#include <QDebug>
#include <QFile>
using namespace std;

class PokerSolver {
public:
    PokerSolver();
    PokerSolver(string ranks,string suits,string compairer_file,int compairer_file_lines,string compairer_file_bin);
    void load_game_tree(string game_tree_file);
    void build_game_tree(
            float oop_commit,
            float ip_commit,
            int current_round,
            int raise_limit,
            float small_blind,
            float big_blind,
            float stack,
            GameTreeBuildingSettings buildingSettings,
            float allin_threshold
    );
    void train(
            string p1_range,
            string p2_range,
            string boards,
            string log_file,
            int iteration_number,
            int print_interval,
            string algorithm,
            int warmup,
            float accuracy,
            bool use_isomorphism,
            int use_halffloats,
            int threads
            );
    void stop();
    long long estimate_tree_memory(QString range1,QString range2,QString board);
    vector<PrivateCards> player1Range;
    vector<PrivateCards> player2Range;
    void dump_strategy(QString dump_file,int dump_rounds);
    shared_ptr<GameTree> get_game_tree(){return this->game_tree;};
    Deck* get_deck(){return &this->deck;}
    shared_ptr<Solver> get_solver(){return this->solver;}
private:
    shared_ptr<Dic5Compairer> compairer;
    Deck deck;
    shared_ptr<GameTree> game_tree;
    shared_ptr<Solver> solver;
public:
    const shared_ptr<GameTree> &getGameTree() const;
};


#endif //TEXASSOLVER_POKERSOLVER_H
