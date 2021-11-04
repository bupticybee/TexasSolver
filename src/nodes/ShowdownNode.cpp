//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "include/nodes/ShowdownNode.h"

#include <utility>

ShowdownNode::ShowdownNode(vector<double> tie_payoffs, vector<vector<double>> player_payoffs,
                           GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode>parent):
                           GameTreeNode(round,pot,std::move(parent)) {
    this->tie_payoffs = std::move(tie_payoffs);
    this->player_payoffs = std::move(player_payoffs);
}


double ShowdownNode::get_payoffs(ShowdownNode::ShowDownResult result, int winner,int player) {
    if(result == ShowDownResult::NOTTIE){
        if(winner == -1) throw runtime_error("winner == -1 in tie");
        return player_payoffs[winner][player];
    }else{
        // if the showdown is a tie
        if(winner != -1) throw runtime_error("winner != -1 in not tie");
        return this->tie_payoffs[player];
    }
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
