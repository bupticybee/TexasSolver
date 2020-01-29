//
// Created by Xuefeng Huang on 2020/1/29.
//

#ifndef TEXASSOLVER_GAMETREENODE_H
#define TEXASSOLVER_GAMETREENODE_H
#include <string>
#include <vector>
#include "fmt/format.h"
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
    GameTreeNode(GameRound round, double pot, GameTreeNode* parent);
    int depth;
    int subtree_size;
    static int gameRound2int(GameRound gameRound);
    GameTreeNode* getParent();
    void setParent(GameTreeNode* parent);
    GameRound getRound();
    double getPot();
    void printHistory();
    static void printNodeHistory(GameTreeNode* node);
    virtual GameTreeNodeType getType() = 0;

private:
    GameRound round;
    double pot;
    GameTreeNode* parent;

};


#endif //TEXASSOLVER_GAMETREENODE_H
