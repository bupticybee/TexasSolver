//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_CHANCENODE_H
#define TEXASSOLVER_CHANCENODE_H


#include <include/Card.h>
#include "include/nodes/GameTreeNode.h"

class ChanceNode:public GameTreeNode {
public:
    ChanceNode(const shared_ptr<GameTreeNode> children, GameRound round, double pot, shared_ptr<GameTreeNode>parent, const vector<Card>& cards,bool donk=false);
    const vector<Card>& getCards();
    shared_ptr<GameTreeNode> getChildren();
    void setChildren(shared_ptr<GameTreeNode> children);
    int getPlayer();
    bool isDonk();

        //vector<vector<vector<float>>> arr_new_reach_probs;
    //vector<vector<vector<float>>> best_respond_arr_new_reach_probs;

private:
    GameTreeNodeType getType() override;
    //Trainable getTrainable();
    //void setTrainable(Trainable trainable);
    // TODO 我感觉这里也可以减肥
    shared_ptr<GameTreeNode> children;
    //Trainable trainable;
    int player{};
    const vector<Card>& cards;
    bool donk;

};


#endif //TEXASSOLVER_CHANCENODE_H
