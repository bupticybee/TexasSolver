//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_CHANCENODE_H
#define TEXASSOLVER_CHANCENODE_H


#include <Card.h>
#include "GameTreeNode.h"

class ChanceNode:public GameTreeNode {
public:
    ChanceNode(const vector<shared_ptr<GameTreeNode>>& childrens, GameRound round, double pot, shared_ptr<GameTreeNode>parent, const vector<Card>& cards);
    vector<Card>& getCards();
    vector<shared_ptr<GameTreeNode>> getChildrens();
    int getPlayer();

private:
    GameTreeNodeType getType() override;
    //Trainable getTrainable();
    //void setTrainable(Trainable trainable);
private:
    vector<shared_ptr<GameTreeNode>> childrens;
    //Trainable trainable;
    int player{};
    vector<Card> cards;

};


#endif //TEXASSOLVER_CHANCENODE_H
