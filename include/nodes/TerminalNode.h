//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_TERMINALNODE_H
#define TEXASSOLVER_TERMINALNODE_H


#include "GameTreeNode.h"

class TerminalNode:public GameTreeNode  {
public:
    TerminalNode();
    TerminalNode(vector<double> payoffs,int winner,GameTreeNode::GameRound round,double pot,shared_ptr<GameTreeNode> parent);
    vector<double> get_payoffs();

private:
    GameTreeNodeType getType() override;

private:
    vector<double> payoffs;
    int winner{};
};


#endif //TEXASSOLVER_TERMINALNODE_H
