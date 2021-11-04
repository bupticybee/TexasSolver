//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_GAMEACTIONS_H
#define TEXASSOLVER_GAMEACTIONS_H

#include <string>
#include <vector>
#include "include/tools/tinyformat.h"
#include "include/nodes/GameTreeNode.h"

class GameActions {
public:
    GameActions();
    GameTreeNode::PokerActions getAction();
    double getAmount();
    GameActions(GameTreeNode::PokerActions action, double amount);
    string toString();
    string pokerActionToString(GameTreeNode::PokerActions pokerActions);
private:
    GameTreeNode::PokerActions action;
    double amount{};

};

#endif //TEXASSOLVER_GAMEACTIONS_H
