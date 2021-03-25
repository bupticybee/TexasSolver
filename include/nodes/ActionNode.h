//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_ACTIONNODE_H
#define TEXASSOLVER_ACTIONNODE_H


#include <trainable/Trainable.h>
#include <thread>
#include <mutex>
#include <ranges/PrivateCards.h>
#include "GameTreeNode.h"
#include "GameActions.h"

class ActionNode :public GameTreeNode {
public:
    ActionNode(vector<GameActions> actions, vector<shared_ptr<GameTreeNode>> childrens, int player, GameRound round,double pot,shared_ptr<GameTreeNode> parent);
    vector<GameActions>& getActions();
    vector<shared_ptr<GameTreeNode>>& getChildrens();
    int getPlayer();
    shared_ptr<Trainable> getTrainable(int i);
    void setTrainable(vector<shared_ptr<Trainable>> trainable,vector<PrivateCards>* player_privates,shared_ptr<ActionNode> action_node);
    vector<PrivateCards>* player_privates;
    shared_ptr<ActionNode> action_node;
    //vector<vector<vector<float>>> arr_new_reach_probs;
    //vector<vector<vector<float>>> best_respond_arr_new_reach_probs;

private:
    GameTreeNodeType getType() override;
private:
    // TODO 这里可以减肥
    vector<GameActions> actions;
    // TODO 这里也可以减肥，不同chance node之后的节点都可以复用,可能通过记录一个id的形式区分不同分支,复用，复用，复用到极致
    vector<shared_ptr<GameTreeNode>> childrens;
    vector<shared_ptr<Trainable>> trainables;
    int player;
};


#endif //TEXASSOLVER_ACTIONNODE_H
