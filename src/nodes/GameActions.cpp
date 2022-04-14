//
// Created by Xuefeng Huang on 2020/1/29.
//

#include "include/nodes/GameActions.h"

GameActions::GameActions() = default;

GameTreeNode::PokerActions GameActions::getAction() {
    return this->action;
}

double GameActions::getAmount() {
    return this->amount;
}

GameActions::GameActions(GameTreeNode::PokerActions action, double amount) {
    this->action = action;
    if (action == GameTreeNode::PokerActions::RAISE || action == GameTreeNode::PokerActions::BET ) {
        if (amount == -1) throw runtime_error(tfm::format("raise/bet amount should not be -1, find %s",amount));
    } else {
        if (amount != -1) throw runtime_error(tfm::format("check/fold/call amount should be -1, find %s",amount));
    }
    this->amount = amount;
}

string GameActions::pokerActionToString(GameTreeNode::PokerActions pokerActions) {
    switch (pokerActions)
    {
        case GameTreeNode::PokerActions::BEGIN :   return "BEGIN";
        case GameTreeNode::PokerActions::ROUNDBEGIN :   return "ROUNDBEGIN";
        case GameTreeNode::PokerActions::BET :   return "BET";
        case GameTreeNode::PokerActions::RAISE :   return "RAISE";
        case GameTreeNode::PokerActions::CHECK :   return "CHECK";
        case GameTreeNode::PokerActions::FOLD :   return "FOLD";
        case GameTreeNode::PokerActions::CALL :   return "CALL";
        default:{
            throw runtime_error("PokerActions not found");
        }
    }
}

string GameActions::toString() {
    if(this->amount == -1) {
        return this->pokerActionToString(this->action);
    }else{
        return this->pokerActionToString(this->action) + " " + to_string(amount);
    }
}
