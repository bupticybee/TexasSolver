//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_SOLVER_H
#define TEXASSOLVER_SOLVER_H


#include <GameTree.h>

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
    virtual json dumps(bool with_status,int depth) = 0;
    shared_ptr<GameTree> tree;
};


#endif //TEXASSOLVER_SOLVER_H
