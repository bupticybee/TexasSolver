//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_SOLVER_H
#define TEXASSOLVER_SOLVER_H


#include <include/GameTree.h>

class Solver {
public:
    enum MonteCarolAlg {
        NONE,
        PUBLIC
    };
    Solver();
    Solver(shared_ptr<GameTree> tree);
    shared_ptr<GameTree> getTree();
    virtual void train() = 0;
    virtual void stop() = 0;
    virtual json dumps(bool with_status,int depth) = 0;
    virtual vector<vector<vector<float>>> get_strategy(shared_ptr<ActionNode> node,vector<Card> cards) = 0;
    virtual vector<vector<vector<float>>> get_evs(shared_ptr<ActionNode> node,vector<Card> cards) = 0;
    shared_ptr<GameTree> tree;
};


#endif //TEXASSOLVER_SOLVER_H
