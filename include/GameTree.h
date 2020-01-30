//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_GAMETREE_H
#define TEXASSOLVER_GAMETREE_H


#include <nodes/GameTreeNode.h>
#include "Deck.h"
#include "json.hpp"
#include <fstream>
#include <nodes/ActionNode.h>
#include <nodes/ShowdownNode.h>
#include <nodes/TerminalNode.h>
#include <nodes/ChanceNode.h>
#include "library.h"

using json = nlohmann::json;

class GameTree {
private:
    string tree_json_dir;
    shared_ptr<GameTreeNode> root = nullptr;
    Deck deck;
public:
    GameTree(string tree_json_dir,Deck deck);;
    shared_ptr<GameTreeNode> getRoot();
    static ifstream readAllBytes(string filePath);
    GameTreeNode::GameRound strToGameRound(string round);
    void recurrentPrintTree(shared_ptr<GameTreeNode> node,int depth,int depth_limit);
    shared_ptr<GameTreeNode> recurrentGenerateTreeNode(json node_json,shared_ptr<GameTreeNode> parent);
    shared_ptr<ActionNode> generateActionNode(json meta,vector<string> childrens_actions, vector<json> childrens_nodes, string round,shared_ptr<GameTreeNode> parent);
    shared_ptr<ChanceNode> generateChanceNode(json meta,json child,string round,shared_ptr<GameTreeNode> parent);
    shared_ptr<ShowdownNode> generateShowdownNode(json meta, string round,shared_ptr<GameTreeNode> parent);
    shared_ptr<TerminalNode> generateTerminalNode(json meta, string round,shared_ptr<GameTreeNode> parent);
    void printTree(int depth);
    json reConvertJson(shared_ptr<GameTreeNode> node);
};


#endif //TEXASSOLVER_GAMETREE_H
