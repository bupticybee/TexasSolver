//
// Created by Xuefeng Huang on 2020/1/31.
//

#include <trainable/CfrPlusTrainable.h>
#include <trainable/DiscountedCfrTrainable.h>
#include "solver/CfrSolver.h"

CfrSolver::CfrSolver(shared_ptr<GameTree> tree, vector<PrivateCards> range1, vector<PrivateCards> range2,
                     vector<int> initial_board, shared_ptr<Compairer> compairer, Deck deck, int iteration_number, bool debug,
                     int print_interval, string logfile, string trainer, Solver::MonteCarolAlg monteCarolAlg) :Solver(tree){
    this->initial_board = initial_board;
    this->initial_board_long = Card::boardInts2long(initial_board);
    this->logfile = logfile;
    this->trainer = trainer;

    range1 = this->noDuplicateRange(range1,initial_board_long);
    range2 = this->noDuplicateRange(range2,initial_board_long);

    this->range1 = range1;
    this->range2 = range2;
    this->player_number = 2;
    this->ranges = vector<vector<PrivateCards>>(this->player_number);
    this->ranges[0] = range1;
    this->ranges[1] = range2;

    this->compairer = compairer;

    this->deck = deck;

    this->rrm = RiverRangeManager(compairer);
    this->iteration_number = iteration_number;

    vector<vector<PrivateCards>> private_cards(this->player_number);
    private_cards[0] = range1;
    private_cards[1] = range2;
    pcm = PrivateCardsManager(private_cards,this->player_number,Card::boardInts2long(this->initial_board));
    this->debug = debug;
    this->print_interval = print_interval;
    this->monteCarolAlg = monteCarolAlg;
}

const vector<PrivateCards> &CfrSolver::playerHands(int player) {
    if(player == 0){
        return range1;
    }else if (player == 1){
        return range2;
    }else{
        throw runtime_error("player not found");
    }
}

vector<vector<float>> CfrSolver::getReachProbs() {
    vector<vector<float>> retval(this->player_number);
    for(int player = 0;player < this->player_number;player ++){
        vector<PrivateCards> player_cards = this->playerHands(player);
        vector<float> reach_prob(player_cards.size());
        for(int i = 0;i < player_cards.size();i ++){
            reach_prob[i] = player_cards[i].weight;
        }
        retval[player] = reach_prob;
    }
    return retval;
}

vector<PrivateCards>
CfrSolver::noDuplicateRange(const vector<PrivateCards> &private_range, uint64_t board_long) {
    vector<PrivateCards> range_array;
    unordered_map<int,bool> rangekv;
    for(PrivateCards one_range:private_range){
        if(rangekv.find(one_range.hashCode()) != rangekv.end())
            throw runtime_error(fmt::format("duplicated key {}",one_range.toString()));
        rangekv[one_range.hashCode()] = true;
        uint64_t hand_long = Card::boardInts2long(one_range.get_hands());
        if(!Card::boardsHasIntercept(hand_long,board_long)){
            range_array.push_back(one_range);
        }
    }
    vector<PrivateCards> ret(range_array.size());
    return ret;

}

void CfrSolver::setTrainable(shared_ptr<GameTreeNode> root) {
    if(root->getType() == GameTreeNode::ACTION){
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(root);

        int player = action_node->getPlayer();
        vector<PrivateCards> player_privates = this->ranges[player];

        if(this->trainer == "cfr_plus"){
            action_node->setTrainable(make_shared<CfrPlusTrainable>(action_node,player_privates));
        }else if(this->trainer == "discounted_cfr"){
            action_node->setTrainable(make_shared<DiscountedCfrTrainable>(action_node,player_privates));
        }

        vector<shared_ptr<GameTreeNode>> childrens =  action_node->getChildrens();
        for(shared_ptr<GameTreeNode> one_child:childrens) setTrainable(one_child);
    }else if(root->getType() == GameTreeNode::CHANCE) {
        shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(root);
        vector<shared_ptr<GameTreeNode>> childrens =  chance_node->getChildrens();
        for(shared_ptr<GameTreeNode> one_child:childrens) setTrainable(one_child);
    }
    else if(root->getType() == GameTreeNode::TERMINAL){

    }else if(root->getType() == GameTreeNode::SHOWDOWN){

    }
}

const vector<float> &CfrSolver::getCardsWeights(int player, vector<float> oppo_reach_probs, uint64_t current_board) {
}

const vector<float> &CfrSolver::cfr(int player, GameTreeNode node, const vector<vector<float>> &reach_probs, int iter,
                                    uint64_t current_board) {
}

const vector<float> &
CfrSolver::chanceUtility(int player, shared_ptr<ChanceNode> node, const vector<vector<float>> &reach_probs, int iter,
                         uint64_t current_board) {
}

const vector<float> &
CfrSolver::actionUtility(int player, shared_ptr<ActionNode> node, const vector<vector<float>> &reach_probs, int iter,
                         uint64_t current_board) {
}

const vector<float> &
CfrSolver::showdownUtility(int player, shared_ptr<ShowdownNode> node, const vector<vector<float>> &reach_probs,
                           int iter, uint64_t current_board) {
}

const vector<float> &
CfrSolver::terminalUtility(int player, shared_ptr<TerminalNode> node, const vector<vector<float>> &reach_prob, int iter,
                           uint64_t current_board) {
}

void CfrSolver::train() {
    setTrainable(this->tree->getRoot());

    //RiverCombs[][] player_rivers = new RiverCombs[this.player_number][];
    // TODO 回头把BestRespond改完之后回头改掉这一块
    //player_rivers[0] = rrm.getRiverCombos(0, this.range1, board);
    //player_rivers[1] = rrm.getRiverCombos(1, this.range2, board);
    vector<vector<PrivateCards>> player_privates(this->player_number);
    player_privates[0] = pcm.getPreflopCards(0);
    player_privates[1] = pcm.getPreflopCards(1);

    //BestResponse br = new BestResponse(player_privates,this.player_number,this.compairer,this.pcm,this.rrm,this.deck,this.debug);

    //br.printExploitability(tree.getRoot(), 0, tree.getRoot().getPot().floatValue(), initial_board_long);

    vector<vector<float>> reach_probs = this->getReachProbs();
    /*
    FileWriter fileWriter = new FileWriter(this.logfile);

    long begintime = System.currentTimeMillis();
    long endtime = System.currentTimeMillis();
    for(int i = 0;i < this.iteration_number;i++){
        for(int player_id = 0;player_id < this.player_number;player_id ++) {
            if(this.debug){
                System.out.println(String.format(
                        "---------------------------------     player %s --------------------------------",
                        player_id
                ));
            }
            this.round_deal = new int[]{-1,-1,-1,-1};
            cfr(player_id,this.tree.getRoot(),reach_probs,i,this.initial_board_long);

        }
        if(i % this.print_interval == 0) {
            System.out.println("-------------------");
            endtime = System.currentTimeMillis();
            float expliotibility = br.printExploitability(tree.getRoot(), i + 1, tree.getRoot().getPot().floatValue(), initial_board_long);
            if(this.logfile != null){
                long time_ms = endtime - begintime;
                JSONObject jo = new JSONObject();
                jo.put("iteration",i);
                jo.put("exploitibility",expliotibility);
                jo.put("time_ms",time_ms);
                fileWriter.write(String.format("%s\n",jo.toJSONString()));
            }
            begintime = System.currentTimeMillis();
        }
    }
    fileWriter.flush();
    fileWriter.close();
    // System.out.println(this.tree.dumps(false).toJSONString());
    */

}

