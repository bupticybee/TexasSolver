//
// Created by Xuefeng Huang on 2020/1/30.
//

#include "GameTree.h"

#include <utility>

GameTree::GameTree(const string& tree_json_dir, Deck deck) {
    this->tree_json_dir = tree_json_dir;
    this->deck = std::move(deck);
    ifstream fs = GameTree::readAllBytes(tree_json_dir);
    json json_content;
    fs >> json_content;
    this->root = this->recurrentGenerateTreeNode(json_content["root"], nullptr);
    this->recurrentSetDepth(this->root,0);
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
        for(shared_ptr<GameTreeNode> one_child:chanceNode->getChildrens()){
            subtree_size += this->recurrentSetDepth(one_child,depth + 1);
        }
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
        throw runtime_error(fmt::format("round {} not found",round));
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
        throw runtime_error(fmt::format("node type {} not found",node_type));
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
        throw runtime_error(fmt::format("node type %s not found",node_type));
    }
}

shared_ptr<ActionNode>
GameTree::generateActionNode(json meta, vector<string> childrens_actions, vector<json> childrens_nodes, const string& round,
                             shared_ptr<GameTreeNode> parent) {
    if(childrens_actions.size() != childrens_nodes.size()){
        throw runtime_error(
                fmt::format(
                        "mismatch when generate ActionNode, childrens_action length {} children_nodes length {}",
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
        throw runtime_error(fmt::format("round {} not found",round));
    }

    vector<GameActions> actions;
    vector<shared_ptr<GameTreeNode>> childrens;

    // 遍历所有children actions 来生成GameAction 的list，用于初始化ActionNode
    for(int i = 0;i < childrens_actions.size();i++){
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
                    throw runtime_error(fmt::format("bet action sp length {}",action_sp.size()));
                string action_str = action_sp[0];
                action = GameTreeNode::PokerActions::BET;
                if(!(action_str == "bet")) throw runtime_error(fmt::format("Action {} not found",action_str));
                amount = stod(action_sp[1]);

            }else if (one_action.find("raise") != string::npos){
                vector<string> action_sp = string_split(one_action,'_');
                if(action_sp.size() != 2)
                    throw runtime_error(fmt::format("raise action sp length {}",action_sp.size()));
                string action_str = action_sp[0];
                action = GameTreeNode::PokerActions::RAISE;
                if(!(action_str == "raise"))
                    throw runtime_error(fmt::format("Action {} not found",action_str));
                amount = stod(action_sp[1]);
            }else{
                throw runtime_error(fmt::format("{} action not found",one_action));
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
        throw runtime_error(fmt::format("childrens length {}, actions length {}"
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
    vector<shared_ptr<GameTreeNode>> childrens;
    for(Card one_card:this->deck.getCards()){
        shared_ptr<GameTreeNode> one_node = recurrentGenerateTreeNode(child, nullptr);
        childrens.push_back(one_node);
    }
    GameTreeNode::GameRound game_round = strToGameRound(std::move(round));
    shared_ptr<ChanceNode> chanceNode = make_shared<ChanceNode>(childrens,game_round,pot,parent,this->deck.getCards());
    for(const shared_ptr<GameTreeNode>& gameTreeNode: chanceNode->getChildrens()){
        gameTreeNode->setParent(chanceNode);
    }
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
    for(int one_player = 0;one_player < player_payoff_list.size();one_player ++){

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
        throw runtime_error(fmt::format("game round not found: {}",round));
    }
    return game_round;
}

void GameTree::recurrentPrintTree(const shared_ptr<GameTreeNode>& node, int depth, int depth_limit) {
    if(depth_limit != -1 && depth >= depth_limit){
        return;
    }

    if(node->getType() == GameTreeNode::ACTION){
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(node);
        vector<shared_ptr<GameTreeNode>> childrens = action_node->getChildrens();
        vector<GameActions> actions = action_node->getActions();

        for(int i = 0;i < childrens.size();i++){
            shared_ptr<GameTreeNode> one_child = childrens[i];
            GameActions one_action = actions[i];

            string prefix;
            for(int j = 0;j < depth;j++) prefix += "\t";
            cout << (fmt::format(
                    "{}p{}: {}",prefix,action_node->getPlayer(),one_action.toString()
            )) << endl;
            recurrentPrintTree(one_child,depth + 1,depth_limit);
        }
    }else if(node->getType() == GameTreeNode::SHOWDOWN){
        shared_ptr<ShowdownNode> showdown_node = std::dynamic_pointer_cast<ShowdownNode>(node);
        string prefix;
        for(int j = 0;j < depth;j++) prefix += "\t";
        cout << (fmt::format(
                "{} SHOWDOWN pot {} ",prefix,showdown_node->getPot()
        )) << endl;

        prefix += "\t";
        for(int i = 0;i < showdown_node->get_payoffs(ShowdownNode::ShowDownResult::TIE,-1).size();i++) {
            cout << (fmt::format("{}if player {} wins, payoff :", prefix,i));
            vector<double> payoffs = showdown_node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE, i);

            for (int player_id = 0; player_id < payoffs.size(); player_id++) {
                cout << (
                        fmt::format(" p{} {} ", player_id, payoffs[player_id])
                );
            }
            cout << endl;
        }
        cout << (fmt::format("{}if Tie, payoff :", prefix));
        vector<double> payoffs = showdown_node->get_payoffs(ShowdownNode::ShowDownResult::TIE, -1);

        for (int player_id = 0; player_id < payoffs.size(); player_id++) {
            cout << (
                    fmt::format(" p{} {} ", player_id, payoffs[player_id])
            );
        }
        cout << endl;
    }else if(node->getType() == GameTreeNode::TERMINAL){
        shared_ptr<TerminalNode> terminal_node = std::dynamic_pointer_cast<TerminalNode>(node);
        string prefix;
        for(int j = 0;j < depth;j++) prefix += "\t";
        cout << (fmt::format(
                "{} TERMINAL pot {} ",prefix,terminal_node->getPot()
        )) << endl;

        prefix += "\t";
        cout << (fmt::format("{}Terminal payoff :", prefix));
        vector<double> payoffs = terminal_node->get_payoffs();

        for (int player_id = 0; player_id < payoffs.size(); player_id++) {
            cout <<(
                    fmt::format("p{} {} ", player_id, payoffs[player_id])
            );
        }
        cout << endl;
    }
}

void GameTree::printTree(int depth) {
    if(depth < -1 || depth == 0){
        throw runtime_error("depth can only be -1 or positive");
    }
    this->recurrentPrintTree(this->root,0,depth);
}

json GameTree::reConvertJson(const shared_ptr<GameTreeNode>& node) {
    if(node->getType() == GameTreeNode::GameTreeNodeType::ACTION) {
        shared_ptr<ActionNode> one_node = std::dynamic_pointer_cast<ActionNode>(node);
        json retjson;

        vector<string> actions_str;
        for(GameActions one_action:one_node->getActions()) actions_str.push_back(one_action.toString());

        retjson["actions"] = actions_str;
        retjson["player"] = one_node->getPlayer();

        json childrens = nullptr;

        for(int i = 0;i < one_node->getActions().size();i ++){
            GameActions one_action = one_node->getActions()[i];
            shared_ptr<GameTreeNode> one_child = one_node->getChildrens()[i];

            json one_json = this->reConvertJson(one_child);
            if(one_json != nullptr) {
                if(childrens == nullptr)childrens = json();
                childrens[one_action.toString()] = std::move(one_json);
            }
        }
        if(childrens != nullptr) {
            retjson["childrens"] = childrens;
        }
        retjson["strategy"] = one_node->getTrainable()->dump_strategy(false);
        retjson["node_type"] = "action_node";
        return std::move(retjson);
    }else if(node->getType() == GameTreeNode::GameTreeNodeType::SHOWDOWN) {
        return nullptr;
    }else if(node->getType() == GameTreeNode::GameTreeNodeType::TERMINAL) {
        return nullptr;
    }else if(node->getType() == GameTreeNode::GameTreeNodeType::CHANCE) {
        shared_ptr<ChanceNode> chanceNode = std::dynamic_pointer_cast<ChanceNode>(node);
        vector<Card> cards = chanceNode->getCards();
        vector<shared_ptr<GameTreeNode>> childerns = chanceNode->getChildrens();
        if(cards.size() != childerns.size())
            throw runtime_error("length not match");
        json retjson;
        vector<string> card_strs;
        for(Card card:cards)
            card_strs.push_back(card.toString());

        json dealcards;
        for(int i = 0;i < cards.size();i ++){
            Card one_card = cards[i];
            shared_ptr<GameTreeNode> gameTreeNode = childerns[i];
            dealcards[one_card.toString()] = this->reConvertJson(gameTreeNode);
        }

        retjson["deal_cards"] = dealcards;
        retjson["deal_number"] = dealcards.size();
        retjson["node_type"] = "chance_node";
        return std::move(retjson);
    }else{
        throw runtime_error("node type unknown!!");
    }
}

json GameTree::dumps(bool with_status) {
    if(with_status == true){
        throw runtime_error("");
    }
    return std::move(this->reConvertJson(this->getRoot()));
}
