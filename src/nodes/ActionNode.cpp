//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "nodes/ActionNode.h"

#include <utility>

ActionNode::ActionNode(vector<GameActions> actions, vector<shared_ptr<GameTreeNode>> childrens, int player,
                       GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode> parent) :GameTreeNode(round,pot,std::move(parent)){
    this->actions = std::move(actions);
    this->player = player;
    this->childrens = std::move(childrens);

}

vector<GameActions>& ActionNode::getActions() {
    return this->actions;
}

vector<shared_ptr<GameTreeNode>>& ActionNode::getChildrens() {
    return this->childrens;
}

int ActionNode::getPlayer() {
    return this->player;
}

GameTreeNode::GameTreeNodeType ActionNode::getType() {
    return ACTION;
}

shared_ptr<Trainable> ActionNode::getTrainable() {
    return this->trainable;
}

void ActionNode::setTrainable(shared_ptr<Trainable> trainable) {
    this->trainable = trainable;
}
