//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "include/nodes/ActionNode.h"

#include <utility>
#include <include/trainable/DiscountedCfrTrainable.h>
#include <include/trainable/DiscountedCfrTrainableHF.h>
#include <include/trainable/DiscountedCfrTrainableSF.h>

ActionNode::~ActionNode(){
    //cout << "ActionNode destroyed" << endl;
}

ActionNode::ActionNode(vector<GameActions> actions, vector<shared_ptr<GameTreeNode>> childrens, int player,
                       GameTreeNode::GameRound round, double pot, shared_ptr<GameTreeNode> parent) :GameTreeNode(round,pot,std::move(parent)){
    this->actions = std::move(actions);
    this->player = player;
    this->childrens = std::move(childrens);
    //cout << "ActionNode created" << endl;
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

shared_ptr<Trainable> ActionNode::getTrainable(int i,bool create_on_site, int use_halffloats) {
    if(i > this->trainables.size()){
        throw runtime_error(tfm::format("size unacceptable %s > %s ",i,this->trainables.size()));
    }
    if(this->trainables[i] == nullptr && create_on_site){
        switch ((this->getRound() == GameTreeNode::RIVER) ? use_halffloats : 0 ){
        case 0:
            this->trainables[i] = make_shared<DiscountedCfrTrainable>(player_privates,*this);
            break;
        case 1:
            this->trainables[i] = make_shared<DiscountedCfrTrainableSF>(player_privates,*this);
            break;
        case 2:
            this->trainables[i] = make_shared<DiscountedCfrTrainableHF>(player_privates,*this);
            break;
        }
    }
    return this->trainables[i];
}

void ActionNode::setTrainable(vector<shared_ptr<Trainable>> trainables,vector<PrivateCards>* player_privates) {
    this->trainables = trainables;
    this->player_privates = player_privates;
}

void ActionNode::setActions(const vector<GameActions> &actions) {
    ActionNode::actions = actions;
}

void ActionNode::setChildrens(const vector<shared_ptr<GameTreeNode>> &childrens) {
    ActionNode::childrens = childrens;
}
