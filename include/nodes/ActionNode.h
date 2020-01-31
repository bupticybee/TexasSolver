//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_ACTIONNODE_H
#define TEXASSOLVER_ACTIONNODE_H


#include <trainable/Trainable.h>
#include "GameTreeNode.h"
#include "GameActions.h"

class ActionNode :public GameTreeNode {
public:
    ActionNode(vector<GameActions> actions, vector<shared_ptr<GameTreeNode>> childrens, int player, GameRound round,double pot,shared_ptr<GameTreeNode> parent);
    vector<GameActions> getActions();
    vector<shared_ptr<GameTreeNode>> getChildrens();
    int getPlayer();
    shared_ptr<Trainable> getTrainable();
    void setTrainable(shared_ptr<Trainable> trainable);

private:
    GameTreeNodeType getType() override;
private:
    vector<GameActions> actions;
    vector<shared_ptr<GameTreeNode>> childrens;
    shared_ptr<Trainable> trainable;
    int player;

};


#endif //TEXASSOLVER_ACTIONNODE_H
