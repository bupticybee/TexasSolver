//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_GAMETREENODE_H
#define TEXASSOLVER_GAMETREENODE_H
#include <string>
#include <vector>
#include <memory>
#include "include/tools/tinyformat.h"
using namespace std;

class GameTreeNode {
public:
    enum PokerActions{
        BEGIN,
        ROUNDBEGIN,
        BET,
        RAISE,
        CHECK,
        FOLD,
        CALL
    };


    enum GameTreeNodeType{
        ACTION,
        SHOWDOWN,
        TERMINAL,
        CHANCE
    };

    enum GameRound{
        PREFLOP,
        FLOP,
        TURN,
        RIVER
    };
    GameTreeNode();
    GameTreeNode(GameRound round, double pot, shared_ptr<GameTreeNode> parent);
    static GameTreeNode::GameRound intToGameRound(int round);
    int depth{};
    int subtree_size{};
    static int gameRound2int(GameRound gameRound);
    shared_ptr<GameTreeNode> getParent();
    void setParent(shared_ptr<GameTreeNode> parent);
    GameRound getRound();
    double getPot();
    void printHistory();
    string toString();
    static void printNodeHistory(GameTreeNode* node);
    virtual GameTreeNodeType getType() = 0;

private:
    GameRound round;
    weak_ptr<GameTreeNode> parent;
    double pot{};
};


#endif //TEXASSOLVER_GAMETREENODE_H
