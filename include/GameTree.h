//
// Created by Xuefeng Huang on 2020/1/30.
//

#ifndef TEXASSOLVER_GAMETREE_H
#define TEXASSOLVER_GAMETREE_H


#include <include/nodes/GameTreeNode.h>
#include "include/Deck.h"
#include "include/json.hpp"
#include <fstream>
#include <include/nodes/ActionNode.h>
#include <include/nodes/ShowdownNode.h>
#include <include/nodes/TerminalNode.h>
#include <include/nodes/ChanceNode.h>
#include "include/library.h"
#include "include/tools/Rule.h"

using json = nlohmann::json;

class GameTree {
private:
    string tree_json_dir;
    shared_ptr<GameTreeNode> root = nullptr;
    Deck deck;
    enum BetType{
        BET,
        DONK,
        RAISE
    };
public:
    GameTree(const string& tree_json_dir,Deck deck);
    GameTree(Deck deck,
            float oop_commit,
            float ip_commit,
            int current_round,
            int raise_limit,
            float small_blind,
            float big_blind,
            float stack,
            GameTreeBuildingSettings buildingSettings,
            float allin_threshold
    );
    ~GameTree();
    shared_ptr<GameTreeNode> __build(shared_ptr<GameTreeNode> node,Rule rule);
    shared_ptr<GameTreeNode> __build(shared_ptr<GameTreeNode> node,Rule rule,string last_action,int check_times,int raise_times);
    void buildChance(shared_ptr<ChanceNode> root,Rule rule);
    void buildAction(shared_ptr<ActionNode> root,Rule rule,string last_action,int check_times,int raise_times);
    shared_ptr<GameTreeNode> getRoot();
    vector<double> get_possible_bets(shared_ptr<ActionNode> root, int player, int next_player, Rule rule, BetType betType);
    double round_nearest(double number, double round_num);
    StreetSetting getSettings(int int_round, int player,GameTreeBuildingSettings& gameTreeBuildingSettings);
    static ifstream readAllBytes(const string& filePath);
    static GameTreeNode::GameRound strToGameRound(const string& round);
    static void recurrentPrintTree(const shared_ptr<GameTreeNode>& node,int depth,int depth_limit);
    long long estimate_tree_memory(int deck_num,int p1range_num,int p2range_num,int num_current_deal=1);
    long long re_estimate_tree_memory(const shared_ptr<GameTreeNode>& node,int deck_num,int p1range_num,int p2range_num,int num_current_deal=1);
    shared_ptr<GameTreeNode> recurrentGenerateTreeNode(json node_json,const shared_ptr<GameTreeNode>& parent);
    shared_ptr<ActionNode> generateActionNode(json meta,vector<string> childrens_actions, vector<json> childrens_nodes, const string& round,shared_ptr<GameTreeNode> parent);
    shared_ptr<ChanceNode> generateChanceNode(json meta,const json& child,string round,shared_ptr<GameTreeNode> parent);
    static shared_ptr<ShowdownNode> generateShowdownNode(json meta, string round,shared_ptr<GameTreeNode> parent);
    shared_ptr<TerminalNode> generateTerminalNode(json meta, string round,shared_ptr<GameTreeNode> parent);
    void printTree(int depth);
    //json dumps(bool with_status,int depth);
private:
    int recurrentSetDepth(shared_ptr<GameTreeNode> node,int depth);
    //void reConvertJson(const shared_ptr<GameTreeNode>& node,json& strategy,string key,int depth,int max_depth,vector<string> prefix,int deal);
};


#endif //TEXASSOLVER_GAMETREE_H
