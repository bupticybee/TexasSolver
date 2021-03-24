//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_SHOWDOWNNODE_H
#define TEXASSOLVER_SHOWDOWNNODE_H


#include "GameTreeNode.h"

class ShowdownNode:public GameTreeNode {
public:
    enum ShowDownResult{
        NOTTIE,TIE
    };
    GameTreeNodeType getType() override;
    ShowdownNode(vector<double> tie_payoffs,vector<vector<double>> player_payoffs,GameRound round,double pot,shared_ptr<GameTreeNode> parent);
    vector<double> get_payoffs(ShowDownResult result,int winner);
    double get_payoffs(ShowDownResult result,int winner,int player);

private:
    // TODO 这里可以减减肥 double -> float
    vector<double> tie_payoffs;
    vector<vector<double>> player_payoffs;
};


#endif //TEXASSOLVER_SHOWDOWNNODE_H
