//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "include/nodes/TerminalNode.h"

TerminalNode::TerminalNode() {

}

TerminalNode::TerminalNode(vector<double> payoffs, int winner, GameTreeNode::GameRound round, double pot,
                           shared_ptr<GameTreeNode> parent):GameTreeNode(round,pot,parent){
    this->payoffs = payoffs;
    this->winner = winner;
}

vector<double> TerminalNode::get_payoffs() {
    return this->payoffs;
}

GameTreeNode::GameTreeNodeType TerminalNode::getType() {
    return TERMINAL;
}
