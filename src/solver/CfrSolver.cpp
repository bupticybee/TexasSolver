//
// Created by Xuefeng Huang on 2020/1/31.
//

#include <solver/BestResponse.h>
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
    return range_array;

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
        }else{
            throw runtime_error(fmt::format("trainer {} not found",this->trainer));
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

const vector<float> &CfrSolver::cfr(int player, shared_ptr<GameTreeNode> node, const vector<vector<float>> &reach_probs, int iter,
                                    uint64_t current_board) {
    switch(node->getType()) {
        case GameTreeNode::ACTION: {
            shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(node);
            return actionUtility(player, action_node, reach_probs, iter, current_board);
        }case GameTreeNode::SHOWDOWN: {
            shared_ptr<ShowdownNode> showdown_node = std::dynamic_pointer_cast<ShowdownNode>(node);
            return showdownUtility(player, showdown_node, reach_probs, iter, current_board);
        }case GameTreeNode::TERMINAL: {
            shared_ptr<TerminalNode> terminal_node = std::dynamic_pointer_cast<TerminalNode>(node);
            return terminalUtility(player, terminal_node, reach_probs, iter, current_board);
        }case GameTreeNode::CHANCE: {
            shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(node);
            return chanceUtility(player, chance_node, reach_probs, iter, current_board);
        }default:
            throw runtime_error("node type unknown");
    }
}

const vector<float> &
CfrSolver::chanceUtility(int player, shared_ptr<ChanceNode> node, const vector<vector<float>> &reach_probs, int iter,
                         uint64_t current_board) {
    vector<Card> cards = this->deck.getCards();
    if(cards.size() != node->getChildrens().size()) throw runtime_error("size mismatch");
    //float[] cardWeights = getCardsWeights(player,reach_probs[1 - player],current_board);

    int card_num = node->getCards().size();
    // 可能的发牌情况,2代表每个人的holecard是两张
    int possible_deals = node->getChildrens().size() - Card::long2board(current_board).size() - 2;

    vector<float> chance_utility(reach_probs[player].size());
    // 遍历每一种发牌的可能性
    // TODO 查为什么PCS的exploitability不为0
    int random_deal = 0,cardcount = 0;
    if(this->monteCarolAlg==MonteCarolAlg::PUBLIC) {
        if (this->round_deal[GameTreeNode::gameRound2int(node->getRound())] == -1) {
            random_deal = random(1, possible_deals + 1 + 2);
            this->round_deal[GameTreeNode::gameRound2int(node->getRound())] = random_deal;
        } else {
            random_deal = this->round_deal[GameTreeNode::gameRound2int(node->getRound())];
        }
    }
    for(int card = 0;card < node->getCards().size();card ++){
        shared_ptr<GameTreeNode> one_child = node->getChildrens()[card];
        Card* one_card  = &(node->getCards()[card]);
        uint64_t card_long = Card::boardInt2long(one_card->getCardInt());//Card::boardCards2long(new Card[]{one_card});

        // 不可能发出和board重复的牌，对吧
        if(Card::boardsHasIntercept(card_long,current_board)) continue;
        cardcount += 1;


        uint64_t new_board_long = current_board | card_long;
        if(this->monteCarolAlg == MonteCarolAlg::PUBLIC){
            if(cardcount == random_deal){
                return this->cfr(player,one_child,reach_probs,iter,new_board_long);
            }else{
                continue;
            }
        }

        vector<PrivateCards>* playerPrivateCard = &(this->ranges[player]);
        vector<PrivateCards>* oppoPrivateCards = &(this->ranges[1 - player]);

        float[][] new_reach_probs = new float[2][];

        new_reach_probs[player] = new float[playerPrivateCard.length];
        new_reach_probs[1 - player] = new float[oppoPrivateCards.length];

        // 检查是否双方 hand和reach prob长度符合要求
        if(playerPrivateCard.length !=reach_probs[player].length) throw new RuntimeException("length not match");
        if(oppoPrivateCards.length !=reach_probs[1 - player].length) throw new RuntimeException("length not match");

        for(int one_player = 0;one_player < 2;one_player ++) {
            int player_hand_len = this.ranges[one_player].length;
            for (int player_hand = 0; player_hand < player_hand_len; player_hand++) {
                PrivateCards one_private = this.ranges[one_player][player_hand];
                uint64_t privateBoardLong = one_private.toBoardLong();
                if (Card.boardsHasIntercept(card_long, privateBoardLong)) continue;
                new_reach_probs[one_player][player_hand] = reach_probs[one_player][player_hand] / possible_deals;
            }
        }

        if(Card.boardsHasIntercept(current_board,card_long))
            throw new RuntimeException("board has intercept with dealt card");

        float[] child_utility = this.cfr(player,one_child,new_reach_probs,iter,new_board_long);
        if(child_utility.length != chance_utility.length) throw new RuntimeException("length not match");
        for(int i = 0;i < child_utility.length;i ++)
            chance_utility[i] += child_utility[i];
    }

    if(this.monteCarolAlg == MonteCarolAlg.PUBLIC) {
        throw new RuntimeException("not possible");
    }
    return chance_utility;
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

    BestResponse br = BestResponse(player_privates,this->player_number,this->pcm,this->rrm,this->deck,this->debug);

    br.printExploitability(tree->getRoot(), 0, tree->getRoot()->getPot(), initial_board_long);

    vector<vector<float>> reach_probs = this->getReachProbs();
    ofstream fileWriter;
    fileWriter.open(this->logfile);

    uint64_t begintime = timeSinceEpochMillisec();
    uint64_t endtime = timeSinceEpochMillisec();
    for(int i = 0;i < this->iteration_number;i++){
        for(int player_id = 0;player_id < this->player_number;player_id ++) {
            if(this->debug){
                cout << (fmt::format(
                        "---------------------------------     player {} --------------------------------",
                        player_id
                )) << endl;
            }
            this->round_deal = vector<int>{-1,-1,-1,-1};
            cfr(player_id,this->tree->getRoot(),reach_probs,i,this->initial_board_long);

        }
        if(i % this->print_interval == 0) {
            cout << ("-------------------") << endl;
            endtime = timeSinceEpochMillisec();
            float expliotibility = br.printExploitability(tree->getRoot(), i + 1, tree->getRoot()->getPot(), initial_board_long);
            if(!this->logfile.empty()){
                long time_ms = endtime - begintime;
                json jo;
                jo["iteration"] = i;
                jo["exploitibility"] = expliotibility;
                jo["time_ms"] = time_ms;
                fileWriter << jo << endl;
            }
            begintime = timeSinceEpochMillisec();
        }
    }
    fileWriter.flush();
    fileWriter.close();
    // System.out.println(this.tree.dumps(false).toJSONString());

}

