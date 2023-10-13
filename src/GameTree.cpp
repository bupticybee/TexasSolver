//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "include/GameTree.h"

#include <utility>

StreetSetting GameTree::getSettings(int int_round, int player,GameTreeBuildingSettings& gameTreeBuildingSettings){
    GameTreeNode::GameRound round = GameTreeNode::intToGameRound(int_round);
    if(!(player == 0 || player == 1)) throw runtime_error(tfm::format("player %s not known",player));
    if(round == GameTreeNode::GameRound::RIVER && player == 0) return gameTreeBuildingSettings.river_ip;
    else if(round == GameTreeNode::GameRound::TURN && player == 0) return gameTreeBuildingSettings.turn_ip;
    else if(round == GameTreeNode::GameRound::FLOP && player == 0) return gameTreeBuildingSettings.flop_ip;
    else if(round == GameTreeNode::GameRound::RIVER && player == 1) return gameTreeBuildingSettings.river_oop;
    else if(round == GameTreeNode::GameRound::TURN && player == 1) return gameTreeBuildingSettings.turn_oop;
    else if(round == GameTreeNode::GameRound::FLOP && player == 1) return gameTreeBuildingSettings.flop_oop;
    else throw new runtime_error(tfm::format("player %s and round not known",player));
}

GameTree::GameTree(const string& tree_json_dir, Deck deck) {
    this->tree_json_dir = tree_json_dir;
    this->deck = std::move(deck);
    ifstream fs = GameTree::readAllBytes(tree_json_dir);
    json json_content;
    fs >> json_content;
    this->root = this->recurrentGenerateTreeNode(json_content["root"], nullptr);
    this->recurrentSetDepth(this->root,0);
}

GameTree::GameTree(Deck deck,
                   float oop_commit,
                   float ip_commit,
                   int current_round,
                   int raise_limit,
                   float small_blind,
                   float big_blind,
                   float stack,
                   GameTreeBuildingSettings buildingSettings,
                   float allin_threshold
){
    Rule rule = Rule(deck,oop_commit,ip_commit,current_round,raise_limit,small_blind,big_blind,stack,buildingSettings,allin_threshold);
    int current_player = 1;
    this->deck = deck;
    GameTreeNode::GameRound round = GameTreeNode::intToGameRound(rule.current_round);
    shared_ptr<ActionNode> node = make_shared<ActionNode>(vector<GameActions>(), vector<shared_ptr<GameTreeNode>>(),current_player, round, (double) rule.get_pot(),
                                 nullptr);
    this->__build(node,rule);
    this->root = node;
}

GameTree::~GameTree(){
    //cout << "root use count: " << this->root.use_count() << endl;
    //cout << "game tree destroyed" << endl;
}

shared_ptr<GameTreeNode> GameTree::__build(shared_ptr<GameTreeNode> node, Rule rule) {
    return this->__build(node,rule,"roundbegin",0,0);
}

shared_ptr<GameTreeNode> GameTree::__build(shared_ptr<GameTreeNode> node, Rule rule, string last_action,
                                           int check_times, int raise_times) {
    switch(node->getType()) {
        case GameTreeNode::ACTION: {
            this->buildAction(std::dynamic_pointer_cast<ActionNode>(node),rule,last_action,check_times,raise_times);
            break;
        }case GameTreeNode::SHOWDOWN: {
            break;
        }case GameTreeNode::TERMINAL: {
            break;
        }case GameTreeNode::CHANCE: {
            this->buildChance(std::dynamic_pointer_cast<ChanceNode>(node),rule);
            break;
        }default:
            throw runtime_error("node type unknown");
    }
    return node;
}

void GameTree::buildChance(shared_ptr<ChanceNode> root,Rule rule){
    //节点上的下注额度
    double pot = (double)rule.get_pot();
    Rule nextrule = Rule(rule);
    if(rule.current_round > 3)throw runtime_error(tfm::format("current round not valid : %d",rule.current_round));

    shared_ptr<GameTreeNode> one_node;
    if(rule.oop_commit == rule.ip_commit && rule.oop_commit == rule.stack) {
        if(rule.current_round >= 3){ // 3 is river
            double p1_commit = rule.ip_commit;
            double p2_commit = rule.oop_commit;
            double peace_getback = (p1_commit + p2_commit) / 2;


            vector<vector<double>> payoffs(2);
            payoffs[0] = {p2_commit, -p2_commit};
            payoffs[1] = {-p1_commit, p1_commit};
            vector<double> peace_getback_vec = {peace_getback - p1_commit, peace_getback - p2_commit};
            one_node = make_shared<ShowdownNode>(peace_getback_vec, payoffs, GameTreeNode::intToGameRound(rule.current_round), (double) rule.get_pot(), root);
        }else {
            nextrule.current_round += 1;
            if(rule.current_round > 3)throw runtime_error(tfm::format("current round not valid : %d",rule.current_round));
            one_node = make_shared<ChanceNode>(nullptr, GameTreeNode::intToGameRound(rule.current_round + 1), (double) rule.get_pot(), root, this->deck.getCards());
        }
    }else {
        one_node = make_shared<ActionNode>(vector<GameActions>(), vector<shared_ptr<GameTreeNode>>(), 1, GameTreeNode::intToGameRound(rule.current_round), (double) rule.get_pot(), root);
    }
    if(rule.current_round > 3)throw runtime_error(tfm::format("current round not valid : %d",rule.current_round));
    this->__build(one_node,nextrule,"begin",0,0);
    root->setChildren(one_node);
}

void GameTree::buildAction(shared_ptr<ActionNode> root,Rule rule,string last_action,int check_times,int raise_times){
    // current player
    int player = root->getPlayer();

    vector<string> possible_actions;
    if(last_action == "roundbegin") {
        possible_actions = vector<string>{"check", "bet"};
    }else if(last_action == "begin") {
        possible_actions = vector<string>{"check", "bet"};
    }else if(last_action == "bet") {
        possible_actions = vector<string>{"call", "raise", "fold"};
    }else if(last_action == "raise") {
        possible_actions = vector<string>{"call", "raise", "fold"};
    }else if(last_action == "check") {
        possible_actions = vector<string>{"check", "raise", "bet"};
    }else if(last_action == "fold") {
        possible_actions = vector<string>{};
    }else if(last_action == "call") {
        possible_actions = vector<string>{"check", "raise"};
    }else{
        throw runtime_error(tfm::format("last action %s not found", last_action));
    }
    int nextplayer = 1 - player;

    vector<GameActions> actions;
    vector<shared_ptr<GameTreeNode>> childrens;

    if (possible_actions.empty()) return;
    for (string action : possible_actions) {
        if (action == "check") {
            // 当不是第一轮的时候 call后面是不能跟check的
            shared_ptr<GameTreeNode> nextnode;
            Rule nextrule = Rule(rule);
            if ((last_action == "call" && root->getParent() != nullptr && root->getParent()->getParent() == nullptr) || check_times >= 1) {
                // 在river check 导致游戏进入showdown
                if(rule.current_round == 3){
                    double p1_commit = rule.ip_commit;
                    double p2_commit = rule.oop_commit;
                    double peace_getback = (p1_commit + p2_commit) / 2;
                    vector<vector<double>> payoffs(2);
                    payoffs[0] = {p2_commit, -p2_commit};
                    payoffs[1] = {-p1_commit, p1_commit};
                    vector<double> peace_getback_vec = {peace_getback - p1_commit, peace_getback - p2_commit};
                    nextnode = make_shared<ShowdownNode>(peace_getback_vec,payoffs,GameTreeNode::intToGameRound(rule.current_round),(double)rule.get_pot(),root);
                }else {
                    // 在preflop/flop/turn check 导致游戏进入下一轮
                    nextrule.current_round += 1;
                    nextnode = make_shared<ChanceNode>(nullptr,GameTreeNode::intToGameRound(rule.current_round + 1), rule.get_pot(), root, this->deck.getCards());
                }
            }else if (root->getParent() == nullptr) {
                nextnode = make_shared<ActionNode>(vector<GameActions>(),vector<shared_ptr<GameTreeNode>>() , nextplayer, GameTreeNode::intToGameRound(rule.current_round), (double) rule.get_pot(), root);
            } else {
                nextnode = make_shared<ActionNode>(vector<GameActions>(),vector<shared_ptr<GameTreeNode>>() , nextplayer, GameTreeNode::intToGameRound(rule.current_round), (double) rule.get_pot(), root);
            }
            this->__build(nextnode, nextrule,"check",check_times + 1,0);
            actions.push_back(GameActions(GameTreeNode::PokerActions::CHECK,-1));
            childrens.push_back(nextnode);
        }else if (action == "bet"){
            BetType betType = BetType::BET;
            // if it's a donk bet
            if(root->getPlayer() == 1 && root->getParent() != nullptr && root->getParent()->getType() == GameTreeNode::GameTreeNodeType::CHANCE){
                shared_ptr<ChanceNode> chanceNodeBeforeThis = std::dynamic_pointer_cast<ChanceNode>(root->getParent());
                if(chanceNodeBeforeThis->isDonk()) betType = BetType::DONK;
            }
            vector<double> bet_sizes = this->get_possible_bets(root,player,nextplayer,rule,betType);
            for(double one_betting_size:bet_sizes){
                Rule nextrule = Rule(rule);
                if (player == 0) nextrule.ip_commit += one_betting_size;
                else if (player == 1) nextrule.oop_commit += one_betting_size;
                else throw runtime_error("unknown player");
                shared_ptr<GameTreeNode> nextnode = make_shared<ActionNode>(vector<GameActions>(), vector<shared_ptr<GameTreeNode>>(),nextplayer,GameTreeNode::intToGameRound(rule.current_round),(double) rule.get_pot(),root);
                this->__build(nextnode,nextrule,"bet",0,1);
                actions.push_back(GameActions(GameTreeNode::PokerActions::BET,one_betting_size));
                childrens.push_back(nextnode);
            }
        }else if (action == "call"){
            Rule nextrule = Rule(rule);
            if (player == 0) nextrule.ip_commit += (rule.oop_commit - rule.ip_commit);
            else if (player == 1) nextrule.oop_commit += (rule.ip_commit - rule.oop_commit);
            else throw runtime_error("unknown player");

            shared_ptr<GameTreeNode> nextnode;
            if(root->getParent() == nullptr) {
                nextnode = make_shared<ActionNode>(vector<GameActions>(), vector<shared_ptr<GameTreeNode>>(), nextplayer, GameTreeNode::intToGameRound(rule.current_round), (double) nextrule.get_pot(), root);
            }else if (rule.current_round == 3 ){ // at river
                double p1_commit = nextrule.ip_commit;
                double p2_commit = nextrule.oop_commit;
                double peace_getback = (p1_commit + p2_commit) / 2;

                vector<vector<double>> payoffs(2);
                payoffs[0] = {p2_commit, -p2_commit};
                payoffs[1] = {-p1_commit, p1_commit};
                vector<double> tie_payoffs = {peace_getback - p1_commit, peace_getback - p2_commit};
                nextnode = make_shared<ShowdownNode>(tie_payoffs,payoffs,GameTreeNode::intToGameRound(rule.current_round),(double) nextrule.get_pot(),root);
            }else{
                nextrule.current_round += 1;
                bool donk = false;
                if(player == 1) donk = true;
                nextnode = make_shared<ChanceNode>(nullptr,GameTreeNode::intToGameRound(rule.current_round + 1),(double) nextrule.get_pot(),root,this->deck.getCards(),donk);
            }
            if(nextrule.current_round > 3)throw runtime_error(tfm::format("round %d exceed 3",nextrule.current_round));
            this->__build(nextnode,nextrule,"call",0,0);
            actions.push_back(GameActions(GameTreeNode::PokerActions::CALL, -1));
            childrens.push_back(nextnode);
        }else if (action == "raise"){
            if(last_action == "call"){
                if(!(root->getParent() != nullptr && root->getParent()->getParent() == nullptr)) continue;
            }else if(last_action == "check"){
                // 第二轮之后的check后面只能跟 bet
                if(!(root->getParent() != nullptr && root->getParent()->getParent() == nullptr && rule.current_round == 0)) continue;
            }
            // 如果raise次数超出限制，则不可以继续raise
            if(raise_times >= rule.raise_limit) continue;
            vector<double> bet_sizes = this->get_possible_bets(root,player,nextplayer,rule,BetType::RAISE);
            for(double one_betting_size:bet_sizes){
                Rule nextrule = Rule(rule);
                if (player == 0) nextrule.ip_commit += one_betting_size;
                else if (player == 1) nextrule.oop_commit += one_betting_size;
                else runtime_error("unknown player");
                shared_ptr<GameTreeNode> nextnode = make_shared<ActionNode>(vector<GameActions>(), vector<shared_ptr<GameTreeNode>>(),nextplayer,GameTreeNode::intToGameRound(rule.current_round),(double) rule.get_pot(),root);
                // Calculate the raise size rather than amount being
                // added to the pot. Do this by ascending the tree until
                // reach a CHANCE node (or no parent) and extract the pot
                // size from it. This can then be used to calculate raise
                shared_ptr<GameTreeNode> roundroot = nextnode;
                while(roundroot->getParent() && roundroot->getType() != GameTreeNode::GameTreeNodeType::CHANCE) {
                    roundroot = roundroot->getParent();
                }
                double raiseto;
                double commit = player == 0 ? nextrule.ip_commit : nextrule.oop_commit;
                if(roundroot) {
                    raiseto = commit - roundroot->getPot()/2;
                } else {
                    raiseto = one_betting_size;
                }
                this->__build(nextnode,nextrule,"raise",0,raise_times + 1);
                actions.push_back(GameActions(GameTreeNode::PokerActions::RAISE,raiseto));
                childrens.push_back(nextnode);
            }

        }else if (action == "fold"){
            Rule nextrule = Rule(rule);
            vector<double> payoffs;
            if (player == 0) {
                payoffs = {-rule.ip_commit, rule.ip_commit};
            }else if(player == 1){
                payoffs = {rule.oop_commit, -rule.oop_commit};
            }else throw runtime_error("unknown player");
            shared_ptr<GameTreeNode> nextnode = make_shared<TerminalNode>(payoffs,nextplayer,GameTreeNode::intToGameRound(rule.current_round), (double) rule.get_pot(),root);
            this->__build(nextnode,nextrule,"fold",0,0);
            actions.push_back(GameActions(GameTreeNode::PokerActions::FOLD,-1));
            childrens.push_back(nextnode);
        }
    }
    if(actions.size() == 0)throw runtime_error("action size is 0");
    root->setActions(actions);
    root->setChildrens(childrens);
}

int GameTree::recurrentSetDepth(shared_ptr<GameTreeNode> node, int depth) {
    node->depth = depth;
    if(node->getType() == GameTreeNode::ACTION) {
        shared_ptr<ActionNode> actionNode = std::dynamic_pointer_cast<ActionNode>(node);
        int subtree_size = 1;
        for(shared_ptr<GameTreeNode> one_child:actionNode->getChildrens()){
            subtree_size += this->recurrentSetDepth(one_child,depth + 1);
        }
        node->subtree_size = subtree_size;
    }else if(node->getType() == GameTreeNode::CHANCE){
        shared_ptr<ChanceNode> chanceNode = std::dynamic_pointer_cast<ChanceNode>(node);
        int subtree_size = 1;
        subtree_size += this->recurrentSetDepth(chanceNode->getChildren(),depth + 1) * chanceNode->getCards().size();
        node->subtree_size = subtree_size;
    }else{
        node->subtree_size = 1;
    }
    return node->subtree_size;
}

shared_ptr<GameTreeNode> GameTree::recurrentGenerateTreeNode(json node_json, const shared_ptr<GameTreeNode>& parent) {
    json meta = node_json["meta"];
    string round = meta["round"];
    if(round.empty()){
        throw runtime_error("node json get round null pointer");
    }

    if(! (round == "preflop"
          || round == "flop"
          || round == "turn"
          || round == "river"
          )
            ){
        throw runtime_error(tfm::format("round %s not found",round));
    }

    string node_type = meta["node_type"];
    if(node_type.empty()){
        throw runtime_error("node json get round null pointer");
    }
    if (! (   node_type == "Terminal"
           || node_type == "Showdown"
           || node_type ==  "Action"
           || node_type ==  "Chance"
    )
            ){
        throw runtime_error(tfm::format("node type %s not found",node_type));
    }

    if(node_type == "Action") {
        // 孩子节点的动作，存在list里
        auto childrens_actions = node_json["children_actions"].get<std::vector<string>>();
        // 孩子节点本身，同样存在list里,和上面的children_actions 一一对应,事实上两者的长度一致
        vector<json> childrens = node_json["children"].get<std::vector<json>>();
        if (childrens.size() != childrens_actions.size()) {
            throw runtime_error("action node child length mismatch");
        }
        return std::dynamic_pointer_cast<GameTreeNode>(this->generateActionNode(meta, childrens_actions, childrens, round,parent));
    }
    else if(node_type == "Showdown") {
        return std::dynamic_pointer_cast<GameTreeNode>(this->generateShowdownNode(meta, round,parent));
    }
    else if(node_type == "Terminal") {
        return std::dynamic_pointer_cast<GameTreeNode>(this->generateTerminalNode(meta, round,parent));
    }
    else if(node_type == "Chance") {
        auto childrens = node_json["children"].get<std::vector<json>>();
        if(childrens.size() != 1) throw runtime_error("Chance node should have only one child");
        return std::dynamic_pointer_cast<GameTreeNode>(this->generateChanceNode(meta, childrens[0], round,parent));
    }
    else{
        throw runtime_error(tfm::format("node type %s not found",node_type));
    }
}

shared_ptr<ActionNode>
GameTree::generateActionNode(json meta, vector<string> childrens_actions, vector<json> childrens_nodes, const string& round,
                             shared_ptr<GameTreeNode> parent) {
    if(childrens_actions.size() != childrens_nodes.size()){
        throw runtime_error(
                tfm::format(
                        "mismatch when generate ActionNode, childrens_action length %s children_nodes length %s",
                        childrens_actions.size(),
                        childrens_nodes.size()
                ));
    }

    if(! (   round == "preflop"
          || round == "flop"
          || round == "turn"
          || round == "river"
          )
            ){
        throw runtime_error(tfm::format("round %s not found",round));
    }

    vector<GameActions> actions;
    vector<shared_ptr<GameTreeNode>> childrens;

    // 遍历所有children actions 来生成GameAction 的list，用于初始化ActionNode
    for(std::size_t i = 0;i < childrens_actions.size();i++){
        string one_action = childrens_actions[i];
        json one_children_map = childrens_nodes[i];
        if(one_action.empty()) throw runtime_error("action is null");

        GameTreeNode::PokerActions action;
        double amount = -1;
        if(one_action == "check"){
            action = GameTreeNode::PokerActions::CHECK;
        }
        else if(one_action == "fold"){
            action = GameTreeNode::PokerActions::FOLD;
        }
        else if(one_action == "call"){
            action = GameTreeNode::PokerActions::CALL;
        }
        else{
            if(one_action.find("bet")  != string::npos){
                vector<string> action_sp = string_split(one_action,'_');
                if(action_sp.size() != 2)
                    throw runtime_error(tfm::format("bet action sp length %s",action_sp.size()));
                string action_str = action_sp[0];
                action = GameTreeNode::PokerActions::BET;
                if(!(action_str == "bet")) throw runtime_error(tfm::format("Action %s not found",action_str));
                amount = stod(action_sp[1]);

            }else if (one_action.find("raise") != string::npos){
                vector<string> action_sp = string_split(one_action,'_');
                if(action_sp.size() != 2)
                    throw runtime_error(tfm::format("raise action sp length %s",action_sp.size()));
                string action_str = action_sp[0];
                action = GameTreeNode::PokerActions::RAISE;
                if(!(action_str == "raise"))
                    throw runtime_error(tfm::format("Action %s not found",action_str));
                amount = stod(action_sp[1]);
            }else{
                throw runtime_error(tfm::format("%s action not found",one_action));
            }
        }
        shared_ptr<GameTreeNode> one_children_node = recurrentGenerateTreeNode(one_children_map,parent);
        childrens.push_back(one_children_node);

        GameActions game_action = GameActions(action,amount);
        actions.push_back(game_action);
    }
    int player = meta["player"];
    double pot = meta["pot"];
    if(childrens.size() != actions.size()){
        throw runtime_error(tfm::format("childrens length %s, actions length %s"
                ,childrens.size()
                ,actions.size()));
    }
    GameTreeNode::GameRound game_round = strToGameRound(round);
    shared_ptr<ActionNode> actionNode = make_shared<ActionNode>(actions,childrens,player,game_round,pot,parent);
    for(const shared_ptr<GameTreeNode>& one_node: childrens){
        one_node->setParent(std::dynamic_pointer_cast<GameTreeNode>(actionNode));
    }
    return actionNode;

}

shared_ptr<ChanceNode>
GameTree::generateChanceNode(json meta, const json& child, string round, shared_ptr<GameTreeNode> parent) {
    //节点上的下注额度
    double pot = meta["pot"];
    shared_ptr<GameTreeNode> one_child = recurrentGenerateTreeNode(child, nullptr);
    GameTreeNode::GameRound game_round = strToGameRound(std::move(round));
    shared_ptr<ChanceNode> chanceNode = make_shared<ChanceNode>(one_child,game_round,pot,parent,this->deck.getCards());
    shared_ptr<GameTreeNode> gameTreeNode = chanceNode->getChildren();
    gameTreeNode->setParent(chanceNode);
    return chanceNode;
}

shared_ptr<ShowdownNode> GameTree::generateShowdownNode(json meta, string round, shared_ptr<GameTreeNode> parent) {
    json meta_payoffs = meta["payoffs"];
    vector<double> tie_payoffs = meta_payoffs["tie"];

    // meta_payoffs 的key有 n个玩家+1个平局,代表某个玩家赢了的时候如何分配payoff
    vector<vector<double>> player_payoffs(2);
    double pot = meta["pot"];

    for(auto one_player_meta=meta_payoffs.begin(); one_player_meta!=meta_payoffs.end(); one_player_meta++){
        const string& one_player = one_player_meta.key();
        if(one_player == "tie"){
            continue;
        }
        // 获胜玩家id
        int player_id = atoi(one_player.c_str());
        if(player_id < 0 or player_id > 1) throw runtime_error("player id json convert fail");

        // 玩家在当前Showdown节点能获得的收益
        //List<Object> tmp_payoffs =  (List<Object>)meta_payoffs.get(one_player);
        vector<double> player_payoff = one_player_meta.value();

        player_payoffs[atoi(one_player.c_str())] = player_payoff;
    }
    GameTreeNode::GameRound game_round = strToGameRound(std::move(round));
    return make_shared<ShowdownNode>(tie_payoffs,player_payoffs,game_round,pot,parent);
}

shared_ptr<TerminalNode> GameTree::generateTerminalNode(json meta, string round, shared_ptr<GameTreeNode> parent) {
    vector<double> player_payoff_list = meta["payoff"];
    vector<double> player_payoff(player_payoff_list.size());
    for(std::size_t one_player = 0;one_player < player_payoff_list.size();one_player ++){

        double tmp_payoff = player_payoff_list[one_player];
        player_payoff[one_player] = tmp_payoff;
    }

    //节点上的下注额度
    double pot = meta["pot"];

    GameTreeNode::GameRound game_round = this->strToGameRound(std::move(round));
    // 多人游戏的时候winner就不等于当前节点的玩家了，这里要注意
    int player = meta["player"];

    return make_shared<TerminalNode>(player_payoff,player,game_round,pot,parent);
}

shared_ptr<GameTreeNode> GameTree::getRoot() {
    return this->root;
}

ifstream GameTree::readAllBytes(const string& filePath) {
    ifstream input_file(filePath);
    return input_file;
}

GameTreeNode::GameRound GameTree::strToGameRound(const string& round) {
    GameTreeNode::GameRound game_round;
    if(round == "preflop"){
        game_round = GameTreeNode::GameRound::PREFLOP;
    }
    else if (round == "flop"){
        game_round = GameTreeNode::GameRound::FLOP;
    }
    else if(round == "turn"){
        game_round = GameTreeNode::GameRound::TURN;
    }
    else if(round == "river"){
        game_round = GameTreeNode::GameRound::RIVER;
    }
    else{
        throw runtime_error(tfm::format("game round not found: %s",round));
    }
    return game_round;
}

long long GameTree::estimate_tree_memory(int deck_num,int p1range_num,int p2range_num,int num_current_deal){
    return this->re_estimate_tree_memory(this->root,deck_num, p1range_num, p2range_num, num_current_deal);
}

long long GameTree::re_estimate_tree_memory(const shared_ptr<GameTreeNode>& node,int deck_num,int p1range_num,int p2range_num,int num_current_deal){
    if(node->getType() == GameTreeNode::ACTION){
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(node);
        vector<shared_ptr<GameTreeNode>> childrens = action_node->getChildrens();
        vector<GameActions> actions = action_node->getActions();

        long long retnum = 0;
        for(std::size_t i = 0;i < childrens.size();i++){
            shared_ptr<GameTreeNode> one_child = childrens[i];
            retnum += re_estimate_tree_memory(one_child,deck_num, p1range_num, p2range_num, num_current_deal);
        }
        if(action_node->getPlayer() == 0){
            retnum += num_current_deal * p1range_num * (childrens.size() + 1) * 3;
        }else{
            retnum += num_current_deal * p2range_num * (childrens.size() + 1) * 3;
        }
        return retnum;
    }else if(node->getType() == GameTreeNode::CHANCE){
        shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(node);
        shared_ptr<GameTreeNode> children = chance_node->getChildren();
        return re_estimate_tree_memory(children,deck_num, p1range_num, p2range_num, num_current_deal * (deck_num));
    }
    return 0;
}

void GameTree::recurrentPrintTree(const shared_ptr<GameTreeNode>& node, int depth, int depth_limit) {
    if(depth_limit != -1 && depth >= depth_limit){
        return;
    }

    if(node->getType() == GameTreeNode::ACTION){
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(node);
        vector<shared_ptr<GameTreeNode>> childrens = action_node->getChildrens();
        vector<GameActions> actions = action_node->getActions();

        for(std::size_t i = 0;i < childrens.size();i++){
            shared_ptr<GameTreeNode> one_child = childrens[i];
            GameActions one_action = actions[i];

            string prefix;
            for(int j = 0;j < depth;j++) prefix += "\t";
            cout << (tfm::format(
                    "%sp%s: %s",prefix,action_node->getPlayer(),one_action.toString()
            )) << endl;
            recurrentPrintTree(one_child,depth + 1,depth_limit);
        }
    }else if(node->getType() == GameTreeNode::SHOWDOWN){
        shared_ptr<ShowdownNode> showdown_node = std::dynamic_pointer_cast<ShowdownNode>(node);
        string prefix;
        for(int j = 0;j < depth;j++) prefix += "\t";
        cout << (tfm::format(
                "%s SHOWDOWN pot %s ",prefix,showdown_node->getPot()
        )) << endl;

        prefix += "\t";
        for(std::size_t i = 0;i < showdown_node->get_payoffs(ShowdownNode::ShowDownResult::TIE,-1).size();i++) {
            cout << (tfm::format("%sif player %s wins, payoff :", prefix,i));
            vector<double> payoffs = showdown_node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE, i);

            for (std::size_t player_id = 0; player_id < payoffs.size(); player_id++) {
                cout << (
                        tfm::format(" p%s %s ", player_id, payoffs[player_id])
                );
            }
            cout << endl;
        }
        cout << (tfm::format("%sif Tie, payoff :", prefix));
        vector<double> payoffs = showdown_node->get_payoffs(ShowdownNode::ShowDownResult::TIE, -1);

        for (std::size_t player_id = 0; player_id < payoffs.size(); player_id++) {
            cout << (
                    tfm::format(" p%s %s ", player_id, payoffs[player_id])
            );
        }
        cout << endl;
    }else if(node->getType() == GameTreeNode::TERMINAL){
        shared_ptr<TerminalNode> terminal_node = std::dynamic_pointer_cast<TerminalNode>(node);
        string prefix;
        for(int j = 0;j < depth;j++) prefix += "\t";
        cout << (tfm::format(
                "%s TERMINAL pot %s ",prefix,terminal_node->getPot()
        )) << endl;

        prefix += "\t";
        cout << (tfm::format("%sTerminal payoff :", prefix));
        vector<double> payoffs = terminal_node->get_payoffs();

        for (std::size_t player_id = 0; player_id < payoffs.size(); player_id++) {
            cout <<(
                    tfm::format("p%s %s ", player_id, payoffs[player_id])
            );
        }
        cout << endl;
    }else if(node->getType() == GameTreeNode::CHANCE){
        shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(node);
        shared_ptr<GameTreeNode> children = chance_node->getChildren();

        string prefix;
        for(int j = 0;j < depth;j++) prefix += "\t";
        cout << (tfm::format(
                "%s%s",prefix,"CHANCE"
        )) << endl;
        recurrentPrintTree(children,depth + 1,depth_limit);
    }
}

void GameTree::printTree(int depth) {
    if(depth < -1 || depth == 0){
        throw runtime_error("depth can only be -1 or positive");
    }
    vector<string> prefix;
    this->recurrentPrintTree(this->root,0,depth);
}

vector<double> GameTree::get_possible_bets(shared_ptr<ActionNode> root, int player, int next_player, Rule rule,
                                           GameTree::BetType betType) {
    if(player != 1 - next_player)throw runtime_error("player error, player and next player not match");
    StreetSetting streetSetting = this->getSettings(root->getRound(),player,rule.build_settings);
    vector<double> bets_ratios;
    bool all_in = streetSetting.allin;
    vector<float> bets_from_rule;
    if(betType == BetType::BET)bets_from_rule = streetSetting.bet_sizes;
    else if(betType == BetType::DONK)bets_from_rule = streetSetting.donk_sizes;
    else if(betType == BetType::RAISE)bets_from_rule = streetSetting.raise_sizes;
    else throw runtime_error("bet type unknown");
    for(float one_bet:bets_from_rule){
        bets_ratios.push_back((double) one_bet / 100);
    }
    float pot = max(rule.ip_commit,rule.oop_commit) * 2;
    vector<double> possible_amounts;
    for(double one_bet: bets_ratios){
        double amount;
        if(rule.oop_commit == rule.small_blind){
            // 当德州扑克开始时，在第一个玩家动作时（sb位置玩家）,视作对手先下注一个bb,这个时候下注要扣除自己的sb
            amount = one_bet * rule.big_blind  - rule.small_blind;
            amount = this->round_nearest(amount, (double) rule.small_blind);
        }else if(rule.ip_commit == rule.big_blind && rule.oop_commit == rule.big_blind){
            // 当德州扑克开始时，在第一个玩家call 的时候第二个玩家要 raise的时候,需要特殊处理
            amount = one_bet * rule.big_blind;
            amount = this->round_nearest(amount, (double) rule.small_blind);
        }else{
            amount = one_bet * pot;
            amount = this->round_nearest(amount, (double) rule.big_blind);
        }
        if(betType == BetType::RAISE)amount += (rule.get_commit(next_player) - rule.get_commit(player));
        if(amount + rule.get_commit(player) > rule.initial_effective_stack * rule.allin_threshold)amount = (double) (rule.stack - rule.get_commit(player)); // if larget than allin thres then allin
        if(amount < rule.stack - rule.get_commit(player) && amount > 0) {
            possible_amounts.push_back(amount);
        }else if(amount == rule.stack - rule.get_commit(player)){
            possible_amounts.push_back(amount);
        }
        else if(amount > rule.stack - rule.get_commit(player)){
            possible_amounts.push_back(rule.stack - rule.get_commit(player));
        }
    }
    if(all_in) possible_amounts.push_back((double) (rule.stack - rule.get_commit(player)));

    if (rule.get_commit(player) != rule.small_blind){
        // 一开始的possible bet amount不能简单取整
        vector<double> tmp_vector;
        for(double val:possible_amounts){
            if(val > 0)tmp_vector.push_back(int(val));
        }
        possible_amounts = tmp_vector;
    }
    if (rule.get_commit(player) == rule.small_blind){
        vector<double> tmp_vector;
        for(double val:possible_amounts){
            if(val >= rule.big_blind)tmp_vector.push_back(val);
        }
        possible_amounts = tmp_vector;
    }else if (rule.get_commit(player) == rule.big_blind && rule.get_commit(next_player) == rule.big_blind){
        vector<double> tmp_vector;
        for(double val:possible_amounts){
            if(val >= rule.big_blind)tmp_vector.push_back(val);
        }
        possible_amounts = tmp_vector;
    }else{
        float gap = rule.get_commit(next_player) - rule.get_commit(player);
        vector<double> tmp_vector;
        for(double val:possible_amounts){
            if(val >= gap * 2)tmp_vector.push_back(val);
        }
        possible_amounts = tmp_vector;
    }
    vector<double> tmp_vector;
    for(double val:possible_amounts){
        if(
                (val > rule.get_commit(next_player) - rule.get_commit(player)) &&
                (val <= rule.stack - rule.get_commit(player)) &&
                find(tmp_vector.begin(),tmp_vector.end(), val) == tmp_vector.end()
        )tmp_vector.push_back(val);
    }
    possible_amounts = tmp_vector;
    return possible_amounts;
}

double GameTree::round_nearest(double number, double round_num) {
    round_num = 1 / round_num;
    return round((number * round_num)) / round_num;

}
