//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_ACTIONNODE_H
#define TEXASSOLVER_ACTIONNODE_H


#include "GameTreeNode.h"
#include "GameActions.h"

class ActionNode :public GameTreeNode {
public:
    ActionNode(vector<GameActions> actions, vector<shared_ptr<GameTreeNode>> childrens, int player, GameRound round,double pot,shared_ptr<GameTreeNode> parent);
    vector<GameActions> getActions();
    vector<shared_ptr<GameTreeNode>> getChildrens();
    int getPlayer();

private:
    GameTreeNodeType getType() override;
    //Trainable getTrainable();
    //void setTrainable(Trainable trainable);
private:
    vector<GameActions> actions;
    vector<shared_ptr<GameTreeNode>> childrens;
    //Trainable trainable;
    int player;

};


#endif //TEXASSOLVER_ACTIONNODE_H
