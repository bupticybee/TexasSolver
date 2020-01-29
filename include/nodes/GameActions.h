//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_GAMEACTIONS_H
#define TEXASSOLVER_GAMEACTIONS_H

#include <string>
#include <vector>
#include "fmt/format.h"
#include "nodes/GameTreeNode.h"

class GameActions {
public:
    GameActions();
    GameTreeNode::PokerActions getAction();
    double getAmount();
    GameActions(GameTreeNode::PokerActions action, double amount);
    string toString();
    const string pokerActionToString(GameTreeNode::PokerActions action);
private:
    GameTreeNode::PokerActions action;
    double amount;

};

#endif //TEXASSOLVER_GAMEACTIONS_H
