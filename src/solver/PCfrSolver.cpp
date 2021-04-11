//
// Created by Xuefeng Huang on 2020/1/31.
//

#include <solver/BestResponse.h>
#include "solver/PCfrSolver.h"

PCfrSolver::PCfrSolver(shared_ptr<GameTree> tree, vector<PrivateCards> range1, vector<PrivateCards> range2,
                     vector<int> initial_board, shared_ptr<Compairer> compairer, Deck deck, int iteration_number, bool debug,
                     int print_interval, string logfile, string trainer, Solver::MonteCarolAlg monteCarolAlg,int warmup,int num_threads) :Solver(tree){
    this->initial_board = initial_board;
    this->initial_board_long = Card::boardInts2long(initial_board);
    this->logfile = logfile;
    this->trainer = trainer;
    this->warmup = warmup;

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
    if(num_threads == -1){
        num_threads = omp_get_num_procs();
    }
    cout << fmt::format("Using {} threads",num_threads) << endl;
    this->num_threads = num_threads;
    omp_set_num_threads(this->num_threads);
    setTrainable(this->tree->getRoot());
}

const vector<PrivateCards> &PCfrSolver::playerHands(int player) {
    if(player == 0){
        return range1;
    }else if (player == 1){
        return range2;
    }else{
        throw runtime_error("player not found");
    }
}

vector<vector<float>> PCfrSolver::getReachProbs() {
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
PCfrSolver::noDuplicateRange(const vector<PrivateCards> &private_range, uint64_t board_long) {
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

void PCfrSolver::setTrainable(shared_ptr<GameTreeNode> root) {
    if(root->getType() == GameTreeNode::ACTION){
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(root);

        int player = action_node->getPlayer();

        if(this->trainer == "cfr_plus"){
            //vector<PrivateCards> player_privates = this->ranges[player];
            //action_node->setTrainable(make_shared<CfrPlusTrainable>(action_node,player_privates));
            throw runtime_error(fmt::format("trainer {} not supported",this->trainer));
        }else if(this->trainer == "discounted_cfr"){
            vector<PrivateCards>* player_privates = &this->ranges[player];
            //action_node->setTrainable(make_shared<DiscountedCfrTrainable>(action_node,player_privates));
            int num;
            GameTreeNode::GameRound gr = this->tree->getRoot()->getRound();
            int root_round = GameTreeNode::gameRound2int(gr);
            int current_round = GameTreeNode::gameRound2int(root->getRound());
            int gap = current_round - root_round;

            if(gap == 2) {
                num = this->deck.getCards().size() * this->deck.getCards().size() +
                          this->deck.getCards().size() + 1;
            }else if(gap == 1) {
                num = this->deck.getCards().size() + 1;
            }else if(gap == 0) {
                num =  1;
            }else{
                throw runtime_error("gap not understand");
            }
            action_node->setTrainable(vector<shared_ptr<Trainable>>(num),player_privates);
        }else{
            throw runtime_error(fmt::format("trainer {} not found",this->trainer));
        }

        vector<shared_ptr<GameTreeNode>> childrens =  action_node->getChildrens();
        for(shared_ptr<GameTreeNode> one_child:childrens) setTrainable(one_child);
    }else if(root->getType() == GameTreeNode::CHANCE) {
        shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(root);
        shared_ptr<GameTreeNode> children = chance_node->getChildren();
        setTrainable(children);
    }
    else if(root->getType() == GameTreeNode::TERMINAL){

    }else if(root->getType() == GameTreeNode::SHOWDOWN){

    }
}

vector<int> PCfrSolver::getAllAbstractionDeal(int deal){
    vector<int> all_deal;
    int card_num = this->deck.getCards().size();
    if(deal == 0){
        all_deal.push_back(deal);
    } else if (deal > 0 && deal <= card_num){
        int origin_deal = int((deal - 1) / 4) * 4;
        for(int i = 0;i < 4;i ++){
            int one_card = origin_deal + i + 1;

            Card *first_card = const_cast<Card *>(&(this->deck.getCards()[origin_deal + i]));
            uint64_t first_long = Card::boardInt2long(
                    first_card->getCardInt());
            if (Card::boardsHasIntercept(first_long, this->initial_board_long))continue;
            all_deal.push_back(one_card);
        }
    } else{
        //cout << "______________________" << endl;
        int c_deal = deal - (1 + card_num);
        int first_deal = int((c_deal / card_num) / 4) * 4;
        int second_deal = int((c_deal % card_num) / 4) * 4;

        for(int i = 0;i < 4;i ++) {
            for(int j = 0;j < 4;j ++) {
                if(first_deal == second_deal && i == j) continue;

                Card *first_card = const_cast<Card *>(&(this->deck.getCards()[first_deal + i]));
                uint64_t first_long = Card::boardInt2long(
                        first_card->getCardInt());
                if (Card::boardsHasIntercept(first_long, this->initial_board_long))continue;

                Card *second_card = const_cast<Card *>(&(this->deck.getCards()[second_deal + j]));
                uint64_t second_long = Card::boardInt2long(
                        second_card->getCardInt());
                if (Card::boardsHasIntercept(second_long, this->initial_board_long))continue;

                int one_card = card_num * (first_deal + i) + (second_deal + j) + 1 + card_num;
                //cout << ";" << this->deck.getCards()[first_deal + i].toString() << "," << this->deck.getCards()[second_deal + j].toString();
                all_deal.push_back(one_card);
            }
        }
        //cout << endl;
    }
    return all_deal;
}



vector<float> PCfrSolver::cfr(int player, shared_ptr<GameTreeNode> node, const vector<float>& reach_probs
                              , int iter, vector<uint64_t> current_board,vector<int> deal){
    switch(node->getType()) {
        case GameTreeNode::ACTION: {
            shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(node);
            return actionUtility(player, action_node, reach_probs, iter, current_board,deal);
        }case GameTreeNode::SHOWDOWN: {
            shared_ptr<ShowdownNode> showdown_node = std::dynamic_pointer_cast<ShowdownNode>(node);
            return showdownUtility(player, showdown_node, reach_probs, iter, current_board,deal);
        }case GameTreeNode::TERMINAL: {
            shared_ptr<TerminalNode> terminal_node = std::dynamic_pointer_cast<TerminalNode>(node);
            return terminalUtility(player, terminal_node, reach_probs, iter, current_board,deal);
        }case GameTreeNode::CHANCE: {
            shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(node);
            return chanceUtility(player, chance_node, reach_probs, iter, current_board,deal);
        }default:
            throw runtime_error("node type unknown");
    }
}

vector<float> PCfrSolver::chanceUtility(int player,shared_ptr<ChanceNode> node,const vector<float>& reach_probs
                            ,int iter,vector<uint64_t> current_boards,vector<int> deals){

    cout << "chance" << endl;
    vector<Card>& cards = this->deck.getCards();
    //float[] cardWeights = getCardsWeights(player,reach_probs[1 - player],current_board);
    int oppo = 1 - player;

    int card_num = node->getCards().size();
    if(card_num % 4 != 0) throw runtime_error("card num cannot round 4");
    // 可能的发牌情况,2代表每个人的holecard是两张
    //vector<vector<vector<float>>> arr_new_reach_probs = vector<vector<vector<float>>>(node->getCards().size());

    //vector<vector<float>> results(node->getCards().size());
    //fill(results.begin(),results.end(),nullptr);

    /*
    vector<float> multiplier;
    if(iter <= this->warmup){
        multiplier = vector<float>(card_num);
        fill(multiplier.begin(), multiplier.end(), 0);
        for (int card_base = 0; card_base < card_num / 4; card_base++) {
            int cardr = std::rand() % 4;
            int card_target = card_base * 4 + cardr;
            int multiplier_num = 0;
            for (int i = 0; i < 4; i++) {
                int i_card = card_base * 4 + i;
                if (i == cardr) {
                    Card *one_card = const_cast<Card *>(&(node->getCards()[i_card]));
                    uint64_t card_long = Card::boardInt2long(
                            one_card->getCardInt());
                    if (!Card::boardsHasIntercept(card_long, current_board)) {
                        multiplier_num += 1;
                    }
                } else {
                    Card *one_card = const_cast<Card *>(&(node->getCards()[i_card]));
                    uint64_t card_long = Card::boardInt2long(
                            one_card->getCardInt());
                    if (!Card::boardsHasIntercept(card_long, current_board)) {
                        multiplier_num += 1;
                    }
                }
            }
            multiplier[card_target] = multiplier_num;
        }
    }
     */
    if(current_boards.size() != deals.size()){
        throw runtime_error("board size deal size not fit");
    }
    if(current_boards.size() * this->ranges[oppo].size() != reach_probs.size()){
        throw runtime_error("board size * range[oppo] and reach prob size not fit");
    }

    shared_ptr<GameTreeNode> one_child = node->getChildren();

    vector<float> new_reach_probs;
    vector<int> original_deals;
    vector<uint64_t> new_board_longs;
    vector<int> new_deals;

    int ind = 0;
    for(int i_deal = 0;i_deal < deals.size();i_deal ++) {
        uint64_t current_board = current_boards[i_deal];
        int deal = deals[i_deal];

        int possible_deals = node->getCards().size() - Card::long2board(current_board).size() - 2;
        int base_deal_ind = i_deal * (this->ranges[oppo].size());

        //vector<float> chance_utility(reach_probs[player].size());
        //#pragma omp parallel for
        for (int card = 0; card < node->getCards().size(); card++) {
            Card *one_card = const_cast<Card *>(&(node->getCards()[card]));
            uint64_t card_long = Card::boardInt2long(
                    one_card->getCardInt());//Card::boardCards2long(new Card[]{one_card});
            if (Card::boardsHasIntercept(card_long, current_board)) continue;
            //if (iter <= this->warmup && multiplier[card] == 0) continue;


            if (this->monteCarolAlg == MonteCarolAlg::PUBLIC) {
                throw runtime_error("parallel solver don't support public monte carol");
            }

            //cout << "Card deal:" << one_card->toString() << endl;

            vector<PrivateCards> &playerPrivateCard = (this->ranges[player]);
            vector<PrivateCards> &oppoPrivateCards = (this->ranges[1 - player]);

            //if (playerPrivateCard.size() != reach_probs[player].size()) throw runtime_error("length not match");
            //if (oppoPrivateCards.size() != reach_probs[1 - player].size()) throw runtime_error("length not match");

            int player_hand_len = this->ranges[oppo].size();
            for (int player_hand = 0; player_hand < player_hand_len; player_hand++) {
                PrivateCards &one_private = this->ranges[oppo][player_hand];
                uint64_t privateBoardLong = one_private.toBoardLong();
                if (Card::boardsHasIntercept(card_long, privateBoardLong)) {
                    new_reach_probs.push_back(0);
                }else {
                    new_reach_probs.push_back(
                            reach_probs[base_deal_ind + player_hand] / possible_deals);
                }
            }
            if (Card::boardsHasIntercept(current_board, card_long))
                throw runtime_error("board has intercept with dealt card");

            int new_deal;
            if (deal == 0) {
                new_deal = card + 1;
            } else if (deal > 0 && deal <= card_num) {
                int origin_deal = deal - 1;
                if (origin_deal == card) throw runtime_error("deal should not be equal");
                new_deal = card_num * origin_deal + card;
                new_deal += (1 + card_num);
            } else {
                throw runtime_error(fmt::format("deal out of range : {} ", deal));
            }
            uint64_t new_board_long = current_board | card_long;
            new_board_longs.push_back(new_board_long);
            new_deals.push_back(new_deal);
            original_deals.push_back(i_deal);
            ind ++;
        }
    }
    vector<float> child_utility = this->cfr(player, one_child, new_reach_probs, iter, new_board_longs, new_deals);
    vector<float> chance_utility = vector<float>(deals.size() * this->ranges[player].size());
    fill(chance_utility.begin(),chance_utility.end(),0);

    if (this->monteCarolAlg == MonteCarolAlg::PUBLIC) {
        throw runtime_error("not possible");
    }
    if(child_utility.size() != new_deals.size() * this->ranges[player].size())

    for(int i = 0;i < new_deals.size();i ++) {
        int origin_deal = original_deals[i];
        for(int j = 0;j < this->ranges[player].size();j ++) {
            int ind = i * this->ranges[player].size() + j;
            chance_utility[origin_deal * this->ranges[player].size() + j] += child_utility[ind];
        }
    }
    cout << "chance end" << endl;
    return chance_utility;
}

vector<float> PCfrSolver::actionUtility(int player,shared_ptr<ActionNode> node,const vector<float>& reach_probs
                            ,int iter,vector<uint64_t> current_boards,vector<int> deals){
    // TODO modify action node to implement new structure
    cout << "action" << endl;
    int oppo = 1 - player;

    if(current_boards.size() != deals.size()){
        throw runtime_error("board size deal size not fit");
    }
    if(current_boards.size() * this->ranges[oppo].size() != reach_probs.size()){
        throw runtime_error("board size * range[oppo] and reach prob size not fit");
    }

    const vector<PrivateCards>& node_player_private_cards = this->ranges[node->getPlayer()];

    vector<float> payoffs = vector<float>(deals.size() * this->ranges[player].size());
    fill(payoffs.begin(),payoffs.end(),0);
    vector<shared_ptr<GameTreeNode>>& children =  node->getChildrens();
    vector<GameActions>& actions =  node->getActions();

    int node_player = node->getPlayer();

    /*
    if(iter <= this->warmup){
        vector<int> deals = this->getAllAbstractionDeal(deal);
        trainable = node->getTrainable(deals[0]);
    }else{
        trainable = node->getTrainable(deal);
    }
     */

    vector<vector<float>> strategys = vector<vector<float>>(deals.size());
    shared_ptr<Trainable> trainable;
    for(int i_deal = 0;i_deal < deals.size();i_deal++) {
        trainable = node->getTrainable(deals[i_deal]);
        strategys[i_deal] = trainable->getcurrentStrategy();
    }

    vector<vector<float>> all_action_utility(actions.size());

    vector<vector<float>> results(actions.size());

    for (int action_id = 0; action_id < actions.size(); action_id++) {
        if(node_player == oppo) {
            //#pragma omp task shared(results,action_id)
            vector<float> new_reach_prob = vector<float>(current_boards.size() * this->ranges[oppo].size());
            fill(new_reach_prob.begin(),new_reach_prob.end(),0);

            for(int i_deal = 0;i_deal < deals.size();i_deal++){
                const vector<float>& current_strategy = strategys[i_deal];
                if (current_strategy.size() != actions.size() * node_player_private_cards.size()) {
                    cout << current_strategy.size() << endl;
                    cout << actions.size() << endl;
                    cout << node_player_private_cards.size() << endl;
                    throw runtime_error(fmt::format(
                            "length not match"
                    ));
                }

                // TODO maybe this code can use some vector optimize
                for (int hand_id = 0; hand_id < node_player_private_cards.size(); hand_id++) {
                    float strategy_prob = current_strategy[action_id * node_player_private_cards.size() + hand_id];
                    new_reach_prob[i_deal * node_player_private_cards.size() + hand_id] = reach_probs[i_deal * node_player_private_cards.size() + hand_id] * strategy_prob;
                }
            }
            results[action_id] = this->cfr(player, children[action_id], new_reach_prob, iter,
                                           current_boards, deals);

        }else{
            results[action_id] = this->cfr(player, children[action_id], reach_probs, iter,
                                           current_boards, deals);
        }
    }

    //#pragma omp taskwait
    for (int action_id = 0; action_id < actions.size(); action_id++) {
        vector<float> action_utilities = results[action_id];
        if (action_utilities.empty()) {
            continue;
        }
        all_action_utility[action_id] = action_utilities;

        // cfr结果是每手牌的收益，payoffs代表的也是每手牌的收益，他们的长度理应相等
        if (action_utilities.size() != payoffs.size()) {
            cout << ("errmsg") << endl;
            cout << (fmt::format("node player {} ", node->getPlayer())) << endl;
            node->printHistory();
            throw runtime_error(
                    fmt::format(
                            "action and payoff length not match {} - {}", action_utilities.size(),
                            payoffs.size()
                    )
            );
        }

        for (int i_deal = 0; i_deal < deals.size(); i_deal++) {
            const vector<float> &current_strategy = strategys[i_deal];
            for (int hand_id = 0; hand_id < node_player_private_cards.size(); hand_id++) {
                if (player == node->getPlayer()) {
                    float strategy_prob = current_strategy[hand_id + action_id * node_player_private_cards.size()];
                    payoffs[i_deal * node_player_private_cards.size() + hand_id] +=
                            strategy_prob * (action_utilities)[i_deal * node_player_private_cards.size() + hand_id];
                } else {
                    payoffs[i_deal * node_player_private_cards.size() + hand_id] += (action_utilities)[
                            i_deal * node_player_private_cards.size() + hand_id];
                }
            }
        }
    }

    if (player == node->getPlayer()) {
        for(int i_deal = 0;i_deal < deals.size();i_deal++) {
            trainable = node->getTrainable(deals[i_deal]);
            vector<float> regrets(actions.size() * node_player_private_cards.size());
            for (int i = 0; i < node_player_private_cards.size(); i++) {
                //boolean regrets_all_negative = true;
                for (int action_id = 0; action_id < actions.size(); action_id++) {
                    // 下面是regret计算的伪代码
                    // regret[action_id * player_hc: (action_id + 1) * player_hc]
                    //     = all_action_utilitiy[action_id] - payoff[action_id]
                    regrets[action_id * node_player_private_cards.size() + i] =
                            (all_action_utility[action_id])[i_deal * node_player_private_cards.size() + i] -
                            payoffs[i_deal * node_player_private_cards.size() + i];
                }
            }
            trainable->updateRegrets(regrets, iter + 1, reach_probs);
        }
        /*if (iter > this->warmup) {
            trainable->updateRegrets(regrets, iter + 1, reach_probs[player]);
        }else if(iter < this->warmup){
        vector<int> deals = this->getAllAbstractionDeal(deal);
        shared_ptr<Trainable> one_trainable = node->getTrainable(deals[0]);
        one_trainable->updateRegrets(regrets, iter + 1, reach_probs[player]);
        }else {
            // iter == this->warmup
            vector<int> deals = this->getAllAbstractionDeal(deal);
            shared_ptr<Trainable> standard_trainable = nullptr;
            for (int one_deal : deals) {
                shared_ptr<Trainable> one_trainable = node->getTrainable(one_deal);
                if (standard_trainable == nullptr) {
                    one_trainable->updateRegrets(regrets, iter + 1, reach_probs[player]);
                    standard_trainable = one_trainable;
                } else {
                    one_trainable->copyStrategy(standard_trainable);
                }
            }
        }*/
    }
    cout << "action end" << endl;
    return payoffs;

}

vector<float> PCfrSolver::showdownUtility(int player,shared_ptr<ShowdownNode> node,const vector<float>& reach_probs
                              ,int iter,vector<uint64_t> current_boards,vector<int> deals){
    // player win时候player的收益，player lose的时候收益明显为-player_payoff
    cout << "showdown" << endl;
    int oppo = 1 - player;
    float win_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE, player, player);
    float lose_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE, oppo, player);
    const vector<PrivateCards> &player_private_cards = this->ranges[player];
    const vector<PrivateCards> &oppo_private_cards = this->ranges[oppo];

    vector<float> payoffs = vector<float>(deals.size() * player_private_cards.size());

    for(int i_deal = 0;i_deal < deals.size();i_deal ++) {
        uint64_t current_board = current_boards[i_deal];

        const vector<RiverCombs> &player_combs = this->rrm.getRiverCombos(player, player_private_cards, current_board);
        const vector<RiverCombs> &oppo_combs = this->rrm.getRiverCombos(oppo, oppo_private_cards, current_board);

        float winsum = 0;
        vector<float> card_winsum = vector<float>(52);//node->card_sum;
        fill(card_winsum.begin(), card_winsum.end(), 0);

        int j = 0;
        for (int i = 0; i < player_combs.size(); i++) {
            const RiverCombs &one_player_comb = player_combs[i];
            while (j < oppo_combs.size() && one_player_comb.rank < oppo_combs[j].rank) {
                const RiverCombs &one_oppo_comb = oppo_combs[j];
                winsum += reach_probs[i_deal * oppo_combs.size() + one_oppo_comb.reach_prob_index];
                card_winsum[one_oppo_comb.private_cards.card1] += reach_probs[i_deal * oppo_combs.size() + one_oppo_comb.reach_prob_index];
                card_winsum[one_oppo_comb.private_cards.card2] += reach_probs[i_deal * oppo_combs.size() + one_oppo_comb.reach_prob_index];
                j++;
            }
            payoffs[i_deal * player_private_cards.size() + one_player_comb.reach_prob_index] = (winsum
                                                         - card_winsum[one_player_comb.private_cards.card1]
                                                         - card_winsum[one_player_comb.private_cards.card2]
                                                        ) * win_payoff;
        }

        // 计算失败时的payoff
        float losssum = 0;
        vector<float> &card_losssum = card_winsum;
        fill(card_losssum.begin(), card_losssum.end(), 0);

        j = oppo_combs.size() - 1;
        for (int i = player_combs.size() - 1; i >= 0; i--) {
            const RiverCombs &one_player_comb = player_combs[i];
            while (j >= 0 && one_player_comb.rank > oppo_combs[j].rank) {
                const RiverCombs &one_oppo_comb = oppo_combs[j];
                losssum += reach_probs[i_deal * oppo_combs.size() + one_oppo_comb.reach_prob_index];
                card_losssum[one_oppo_comb.private_cards.card1] += reach_probs[i_deal * oppo_combs.size() + one_oppo_comb.reach_prob_index];
                card_losssum[one_oppo_comb.private_cards.card2] += reach_probs[i_deal * oppo_combs.size() + one_oppo_comb.reach_prob_index];
                j--;
            }
            payoffs[i_deal * player_private_cards.size() + one_player_comb.reach_prob_index] += (losssum
                                                          - card_losssum[one_player_comb.private_cards.card1]
                                                          - card_losssum[one_player_comb.private_cards.card2]
                                                         ) * lose_payoff;
        }
    }
    cout << "showdown end" << endl;
    return payoffs;
}

vector<float> PCfrSolver::terminalUtility(int player,shared_ptr<TerminalNode> node,const vector<float>& reach_prob
                                          ,int iter,vector<uint64_t> current_boards,vector<int> deals){
    cout << "terminal" << endl;
    float player_payoff = node->get_payoffs()[player];

    int oppo = 1 - player;
    const vector<PrivateCards>& player_hand = playerHands(player);
    const vector<PrivateCards>& oppo_hand = playerHands(oppo);

    vector<float> payoffs = vector<float>(deals.size() * player_hand.size());

    for(int i_deal = 0;i_deal < deals.size();i_deal ++) {
        uint64_t current_board = current_boards[i_deal];
        float oppo_sum = 0;
        vector<float> oppo_card_sum = vector<float>(52);
        fill(oppo_card_sum.begin(), oppo_card_sum.end(), 0);

        for (int i = 0; i < oppo_hand.size(); i++) {
            oppo_card_sum[oppo_hand[i].card1] += reach_prob[i_deal * oppo_hand.size() + i];
            oppo_card_sum[oppo_hand[i].card2] += reach_prob[i_deal * oppo_hand.size() + i];
            oppo_sum += reach_prob[i_deal * oppo_hand.size() + i];
        }

        for (int i = 0; i < player_hand.size(); i++) {
            const PrivateCards &one_player_hand = player_hand[i];
            if (Card::boardsHasIntercept(current_board, Card::boardInts2long(one_player_hand.get_hands()))) {
                continue;
            }
            int oppo_same_card_ind = this->pcm.indPlayer2Player(player, oppo, i);
            float plus_reach_prob;
            if (oppo_same_card_ind == -1) {
                plus_reach_prob = 0;
            } else {
                plus_reach_prob = reach_prob[i_deal * oppo_hand.size() + oppo_same_card_ind];
            }
            payoffs[i_deal * player_hand.size() + i] = player_payoff * (
                    oppo_sum - oppo_card_sum[one_player_hand.card1]
                    - oppo_card_sum[one_player_hand.card2]
                    + plus_reach_prob
            );
        }
    }

    cout << "terminal end" << endl;
    return payoffs;
}

void PCfrSolver::findGameSpecificIsomorphisms() {
    vector<int> board = this->initial_board;
    // TODO maybe use isomorphisms of texas holdem itself to do some kind of speedup
}

void PCfrSolver::purnTree() {
    // TODO how to purn the tree, use wramup to start training in memory-save mode, and switch to purn tree directly to both save memory and speedup
}

void PCfrSolver::train() {

    vector<vector<PrivateCards>> player_privates(this->player_number);
    player_privates[0] = pcm.getPreflopCards(0);
    player_privates[1] = pcm.getPreflopCards(1);

    BestResponse br = BestResponse(player_privates,this->player_number,this->pcm,this->rrm,this->deck,this->debug,this->num_threads);

    //br.printExploitability(tree->getRoot(), 0, tree->getRoot()->getPot(), initial_board_long);

    vector<vector<float>> reach_probs = this->getReachProbs();
    ofstream fileWriter;
    fileWriter.open(this->logfile);
    this->findGameSpecificIsomorphisms();

    uint64_t begintime = timeSinceEpochMillisec();
    uint64_t endtime = timeSinceEpochMillisec();

    for(int i = 0;i < this->iteration_number;i++){
        for(int player_id = 0;player_id < this->player_number;player_id ++) {
            this->round_deal = vector<int>{-1,-1,-1,-1};
            //#pragma omp parallel
            {
                //#pragma omp single
                {
                    cfr(player_id, this->tree->getRoot(), reach_probs[1 - player_id], i, vector<uint64_t>{this->initial_board_long},vector<int>{0});
                }
            }
        }
        if(i % this->print_interval == 0 && i != 0 && i >= this->warmup) {
            endtime = timeSinceEpochMillisec();
            long time_ms = endtime - begintime;
            cout << ("-------------------") << endl;
            float expliotibility = br.printExploitability(tree->getRoot(), i + 1, tree->getRoot()->getPot(), initial_board_long);
            cout << "time used: " << float(time_ms) / 1000 << endl;
            if(!this->logfile.empty()){
                json jo;
                jo["iteration"] = i;
                jo["exploitibility"] = expliotibility;
                jo["time_ms"] = time_ms;
                fileWriter << jo << endl;
            }
            //begintime = timeSinceEpochMillisec();
        }
    }
    fileWriter.flush();
    fileWriter.close();
    // System.out.println(this.tree.dumps(false).toJSONString());

}

