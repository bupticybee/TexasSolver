//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "nodes/ShowdownNode.h"

ShowdownNode::ShowdownNode(vector<double> tie_payoffs, vector<vector<double>> player_payoffs,
                           GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode>parent):
                           GameTreeNode(round,pot,parent) {
    this->tie_payoffs = tie_payoffs;
    this->player_payoffs = player_payoffs;
}

vector<double> ShowdownNode::get_payoffs(ShowdownNode::ShowDownResult result, int winner) {
    if(result == ShowDownResult::NOTTIE){
        if(winner == -1) throw runtime_error("winner == -1 in tie");
        vector<double> retval = player_payoffs[winner];
        return retval;
    }else{
        // if the showdown is a tie
        if(winner != -1) throw runtime_error("winner != -1 in not tie");
        return this->tie_payoffs;
    }
}

GameTreeNode::GameTreeNodeType ShowdownNode::getType() {
    return SHOWDOWN;
}
