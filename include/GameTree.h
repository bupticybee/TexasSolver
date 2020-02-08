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
    GameTree(const string& tree_json_dir,Deck deck);;
    shared_ptr<GameTreeNode> getRoot();
    static ifstream readAllBytes(const string& filePath);
    static GameTreeNode::GameRound strToGameRound(const string& round);
    static void recurrentPrintTree(const shared_ptr<GameTreeNode>& node,int depth,int depth_limit);
    shared_ptr<GameTreeNode> recurrentGenerateTreeNode(json node_json,const shared_ptr<GameTreeNode>& parent);
    shared_ptr<ActionNode> generateActionNode(json meta,vector<string> childrens_actions, vector<json> childrens_nodes, const string& round,shared_ptr<GameTreeNode> parent);
    shared_ptr<ChanceNode> generateChanceNode(json meta,const json& child,string round,shared_ptr<GameTreeNode> parent);
    static shared_ptr<ShowdownNode> generateShowdownNode(json meta, string round,shared_ptr<GameTreeNode> parent);
    shared_ptr<TerminalNode> generateTerminalNode(json meta, string round,shared_ptr<GameTreeNode> parent);
    void printTree(int depth);
    json dumps(bool with_status);
private:
    int recurrentSetDepth(shared_ptr<GameTreeNode> node,int depth);
    json reConvertJson(const shared_ptr<GameTreeNode>& node);
};


#endif //TEXASSOLVER_GAMETREE_H
