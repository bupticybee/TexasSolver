//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "include/solver/Solver.h"

Solver::Solver() {

}

Solver::Solver(shared_ptr<GameTree> tree, Logger *logger):logger(logger) {
    this->tree = tree;
}

shared_ptr<GameTree> Solver::getTree() {
    return this->tree;
}
