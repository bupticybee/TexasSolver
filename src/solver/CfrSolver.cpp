//
// Created by Xuefeng Huang on 2020/1/31.
//

#include <solver/BestResponse.h>
#include "solver/CfrSolver.h"

CfrSolver::CfrSolver(shared_ptr<GameTree> tree, vector<PrivateCards> range1, vector<PrivateCards> range2,
                     vector<int> initial_board, shared_ptr<Compairer> compairer, Deck deck, int iteration_number, bool debug,
                     int print_interval, string logfile, string trainer, Solver::MonteCarolAlg monteCarolAlg) :Solver(tree){

}

