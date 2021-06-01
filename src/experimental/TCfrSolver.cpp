//
// Created by Xuefeng Huang on 2020/1/31.
//

#include "experimental/TCfrSolver.h"

TCfrSolver::TCfrSolver(shared_ptr<GameTree> tree, vector<PrivateCards> range1, vector<PrivateCards> range2,
                     vector<int> initial_board, shared_ptr<Compairer> compairer, Deck deck, int iteration_number, bool debug,
                     int print_interval, string logfile, string trainer, Solver::MonteCarolAlg monteCarolAlg,int num_threads) :Solver(tree){
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
    if(num_threads == -1){
        num_threads = boost::thread::hardware_concurrency() + 1;
    }
    this->num_threads = num_threads;
    this->pool = make_shared<boost::basic_thread_pool>(this->num_threads);
}

const vector<PrivateCards> &TCfrSolver::playerHands(int player) {
    if(player == 0){
        return range1;
    }else if (player == 1){
        return range2;
    }else{
        throw runtime_error("player not found");
    }
}

vector<vector<float>> TCfrSolver::getReachProbs() {
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
TCfrSolver::noDuplicateRange(const vector<PrivateCards> &private_range, uint64_t board_long) {
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

void TCfrSolver::setTrainable(shared_ptr<GameTreeNode> root) {
    if(root->getType() == GameTreeNode::ACTION){
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(root);

        int player = action_node->getPlayer();
        vector<PrivateCards> player_privates = this->ranges[player];

        if(this->trainer == "cfr_plus"){
            //action_node->setTrainable(make_shared<CfrPlusTrainable>(action_node,player_privates));
            throw runtime_error(fmt::format("trainer {} not supported now",this->trainer));
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

const vector<float>* TCfrSolver::cfr(int player, shared_ptr<GameTreeNode> node, const vector<vector<float>> &reach_probs, int iter,
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

const vector<float>*
TCfrSolver::chanceUtility(int player, shared_ptr<ChanceNode> node, const vector<vector<float>> &reach_probs, int iter,
                         uint64_t current_board) {
    vector<Card>& cards = this->deck.getCards();
    if(cards.size() != node->getChildrens().size()) throw runtime_error("size mismatch");
    //float[] cardWeights = getCardsWeights(player,reach_probs[1 - player],current_board);

    int card_num = node->getCards().size();
    // 可能的发牌情况,2代表每个人的holecard是两张
    int possible_deals = node->getChildrens().size() - Card::long2board(current_board).size() - 2;

    //vector<float> chance_utility(reach_probs[player].size());
    if(node->utilities.empty()) {
        node->utilities = vector<vector<float>>(2);
    }
    if(node->utilities[player].empty()) {
        node->utilities[player] = vector<float>(reach_probs[player].size());
    }
    //vector<float>& payoffs = node->utilities;
    vector<float>& chance_utility = node->utilities[player];
    fill(chance_utility.begin(),chance_utility.end(),0);

    int random_deal = 0,cardcount = 0;
    if(this->monteCarolAlg==MonteCarolAlg::PUBLIC) {
        if (this->round_deal[GameTreeNode::gameRound2int(node->getRound())] == -1) {
            random_deal = random(1, possible_deals + 1 + 2);
            this->round_deal[GameTreeNode::gameRound2int(node->getRound())] = random_deal;
        } else {
            random_deal = this->round_deal[GameTreeNode::gameRound2int(node->getRound())];
        }
    }
    if(node->arr_new_reach_probs.empty()){
        node->arr_new_reach_probs = vector<vector<vector<float>>>(node->getCards().size());
    }

    for(int card = 0;card < node->getCards().size();card ++) {
        shared_ptr<GameTreeNode> one_child = node->getChildrens()[card];
        Card *one_card = &(node->getCards()[card]);
        uint64_t card_long = Card::boardInt2long(one_card->getCardInt());//Card::boardCards2long(new Card[]{one_card});
        if(Card::boardsHasIntercept(card_long,current_board)) continue;
        cardcount += 1;

        uint64_t new_board_long = current_board | card_long;
        if(this->monteCarolAlg == MonteCarolAlg::PUBLIC){
            throw runtime_error("parallel solver don't support public monte carol");
        }

        vector<PrivateCards>& playerPrivateCard = (this->ranges[player]);
        vector<PrivateCards>& oppoPrivateCards = (this->ranges[1 - player]);

        if(node->arr_new_reach_probs[card].empty()){
            node->arr_new_reach_probs[card] = vector<vector<float>>(2);
        }
        vector<vector<float>>& new_reach_probs = node->arr_new_reach_probs[card];
        if(new_reach_probs[player].empty()) {
            new_reach_probs[player] = vector<float>(playerPrivateCard.size());
            new_reach_probs[1 - player] = vector<float>(oppoPrivateCards.size());
        }


        if(playerPrivateCard.size() !=reach_probs[player].size()) throw runtime_error("length not match");
        if(oppoPrivateCards.size() !=reach_probs[1 - player].size()) throw runtime_error("length not match");

        for(int one_player = 0;one_player < 2;one_player ++) {
            int player_hand_len = this->ranges[one_player].size();
            for (int player_hand = 0; player_hand < player_hand_len; player_hand++) {
                PrivateCards& one_private = this->ranges[one_player][player_hand];
                uint64_t privateBoardLong = one_private.toBoardLong();
                if (Card::boardsHasIntercept(card_long, privateBoardLong)) {
                    new_reach_probs[one_player][player_hand] = 0;
                    continue;
                }
                new_reach_probs[one_player][player_hand] = reach_probs[one_player][player_hand] / possible_deals;
            }
        }
        if(Card::boardsHasIntercept(current_board,card_long))
            throw runtime_error("board has intercept with dealt card");
        const vector<float>* child_utility = this->cfr(player,one_child,new_reach_probs,iter,new_board_long);
        if(child_utility->size() != chance_utility.size()) throw runtime_error("length not match");
        for(int i = 0;i < child_utility->size();i ++)
            chance_utility[i] += (*child_utility)[i];
    }
    if(this->monteCarolAlg == MonteCarolAlg::PUBLIC) {
        throw runtime_error("not possible");
    }
    return &chance_utility;
}

const vector<float> *
TCfrSolver::actionUtility(int player, shared_ptr<ActionNode> node, const vector<vector<float>> &reach_probs, int iter,
                         uint64_t current_board) {
    int oppo = 1 - player;
    const vector<PrivateCards>& node_player_private_cards = this->ranges[node->getPlayer()];
    shared_ptr<Trainable> trainable = node->getTrainable();

    if(node->utilities.empty()) {
        node->utilities = vector<vector<float>>(2);
    }
    if(node->utilities[player].empty()) {
        node->utilities[player] = vector<float>(this->ranges[player].size());
    }
    vector<float>& payoffs = node->utilities[player];
    fill(payoffs.begin(),payoffs.end(),0);
    vector<shared_ptr<GameTreeNode>>& children =  node->getChildrens();
    vector<GameActions>& actions =  node->getActions();

    const vector<float>& current_strategy = trainable->getcurrentStrategy();
    if(this->debug){
        for(float one_strategy:current_strategy){
            // when one_strategy is nan
            if(one_strategy != one_strategy) {
                throw runtime_error("strategy contains nan");
            }

        }
        for(int one_player = 0;one_player < this->player_number;one_player ++){
            ;
            for(float one_prob:reach_probs[one_player]){
                if(one_prob != one_prob)
                    throw runtime_error("prob nan");
            }
        }
    }
    if (current_strategy.size() != actions.size() * node_player_private_cards.size()) {
        node->printHistory();
        throw runtime_error(fmt::format(
                "length not match {} - {} \n action size {} private_card size {}"
                ,current_strategy.size()
                ,actions.size() * node_player_private_cards.size()
                ,actions.size()
                ,node_player_private_cards.size()
        ));
    }

    if(node->arr_new_reach_probs.empty()){
        node->arr_new_reach_probs = vector<vector<vector<float>>>(actions.size());
    }

    //为了节省计算成本将action regret 存在一位数组而不是二维数组中，两个纬度分别是（该infoset有多少动作,该palyer有多少holecard）
    vector<float> regrets(actions.size() * node_player_private_cards.size());

    vector<const vector<float> *> all_action_utility(actions.size());
    int node_player = node->getPlayer();

    vector<boost::future<const vector<float>*>> results(actions.size());
    //fill(results.begin(),results.end(),nullptr);
    for (int action_id = 0; action_id < actions.size(); action_id++) {

        if (node->arr_new_reach_probs[action_id].empty()) {
            node->arr_new_reach_probs[action_id] = vector<vector<float>>(2);
        }
        vector<vector<float>> &new_reach_prob = node->arr_new_reach_probs[action_id];
        if (new_reach_prob[player].empty()) {
            new_reach_prob[player] = vector<float>(reach_probs[player].size());
            new_reach_prob[1 - player] = vector<float>(reach_probs[1 - player].size());
        }

        for (int hand_id = 0; hand_id < new_reach_prob[node_player].size(); hand_id++) {
            float strategy_prob = current_strategy[hand_id + action_id * node_player_private_cards.size()];
            new_reach_prob[node_player][hand_id] = reach_probs[node_player][hand_id] * strategy_prob;
        }

        new_reach_prob[1 - node_player].assign(reach_probs[1 - node_player].begin(),
                                               reach_probs[1 - node_player].end());

        shared_ptr<GameTreeNode> nextnode = children[action_id];
        results[action_id] = boost::async(
            [&]()->const vector<float> * {
                    return this->cfr(player,nextnode , new_reach_prob, iter,
                              current_board);
                }
            );
    }

    for (int action_id = 0; action_id < actions.size(); action_id++) {
        boost::future<const vector<float>*>& action_utilities_result = results[action_id];
        this->pool->reschedule_until([&]()->bool{
            return action_utilities_result.is_ready();
        });
        const vector<float>* action_utilities = action_utilities_result.get();

        if(action_utilities == nullptr){
            continue;
        }
        all_action_utility[action_id] = action_utilities;

        // cfr结果是每手牌的收益，payoffs代表的也是每手牌的收益，他们的长度理应相等
        if (action_utilities->size() != payoffs.size()) {
            cout << ("errmsg") << endl;
            cout << (fmt::format("node player {} ", node->getPlayer())) << endl;
            node->printHistory();
            throw runtime_error(
                    fmt::format(
                            "action and payoff length not match {} - {}", action_utilities->size(),
                            payoffs.size()
                    )
            );
        }

        for (int hand_id = 0; hand_id < action_utilities->size(); hand_id++) {
            if (player == node->getPlayer()) {
                float strategy_prob = current_strategy[hand_id + action_id * node_player_private_cards.size()];
                payoffs[hand_id] += strategy_prob * (*action_utilities)[hand_id];
            } else {
                payoffs[hand_id] += (*action_utilities)[hand_id];
            }
        }
    }


    if (player == node->getPlayer()) {
        for (int i = 0; i < node_player_private_cards.size(); i++) {
            //boolean regrets_all_negative = true;
            for (int action_id = 0; action_id < actions.size(); action_id++) {
                // 下面是regret计算的伪代码
                // regret[action_id * player_hc: (action_id + 1) * player_hc]
                //     = all_action_utilitiy[action_id] - payoff[action_id]
                regrets[action_id * node_player_private_cards.size() + i] =
                        (*all_action_utility[action_id])[i] - payoffs[i];
            }
        }
        trainable->updateRegrets(regrets, iter + 1, reach_probs[player]);
    }
    //if(this.debug && player == node.getPlayer()) {

    return &payoffs;

}

const vector<float>*
TCfrSolver::showdownUtility(int player, shared_ptr<ShowdownNode> node, const vector<vector<float>> &reach_probs,
                           int iter, uint64_t current_board) {
    // player win时候player的收益，player lose的时候收益明显为-player_payoff
    int oppo = 1 - player;
    float win_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE,player,player);
    float lose_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE,oppo,player);
    const vector<PrivateCards>& player_private_cards = this->ranges[player];
    const vector<PrivateCards>& oppo_private_cards = this->ranges[oppo];

    const vector<RiverCombs>& player_combs = this->rrm.getRiverCombos(player,player_private_cards,current_board);
    const vector<RiverCombs>& oppo_combs = this->rrm.getRiverCombos(oppo,oppo_private_cards,current_board);

    if(node->utilities.empty()) {
        node->utilities = vector<vector<float>>(2);
    }
    if(node->utilities[player].empty()) {
        node->utilities[player] = vector<float>(player_private_cards.size());
    }
    vector<float>& payoffs = node->utilities[player];

    float winsum = 0;
    if(node->card_sum.empty()){
        node->card_sum = vector<float> (52);
    }
    vector<float>& card_winsum = node->card_sum;
    fill(card_winsum.begin(),card_winsum.end(),0);

    int j = 0;
    //if(player_combs.length != oppo_combs.length) throw new RuntimeException("");

    /*
    if(this->debug){
        cout << ("[PRESHOWDOWN]=======================");
        cout << ("preflop combos: ") << endl;
        for(RiverCombs one_river_comb:player_combs){
            cout << (fmt::format("{}({}) "
                    ,one_river_comb.private_cards.toString()
                    ,one_river_comb.rank
            )) << endl;
        }
        cout << endl;
    }
    */

    for(int i = 0;i < player_combs.size();i ++){
        const RiverCombs& one_player_comb = player_combs[i];
        while (j < oppo_combs.size() && one_player_comb.rank < oppo_combs[j].rank){
            const RiverCombs& one_oppo_comb = oppo_combs[j];
            winsum += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            /*
            if(this->debug) {
                if (one_player_comb.reach_prob_index == 0) {
                    cout << (fmt::format("[{}]:{}-{}({}) "
                            ,j
                            ,this->ranges[oppo][one_oppo_comb.reach_prob_index].weight
                            ,winsum
                            ,one_oppo_comb.rank
                    )) << endl;
                }
            }
            */

            // TODO 这里有问题，要加上reach prob，但是reach prob的index怎么解决？
            card_winsum[one_oppo_comb.private_cards.card1] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            card_winsum[one_oppo_comb.private_cards.card2] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            j ++;
        }
        /*
        if(this->debug){
            cout << (fmt::format("Before Adding {}, win_payoff {} winsum {}, subcard1 {} subcard2 {}"
                    ,payoffs[one_player_comb.reach_prob_index]
                    ,win_payoff
                    ,winsum
                    ,- card_winsum[one_player_comb.private_cards.card1]
                    ,- card_winsum[one_player_comb.private_cards.card2]
            )) << endl;
        }
         */
        payoffs[one_player_comb.reach_prob_index] = (winsum
                                                     - card_winsum[one_player_comb.private_cards.card1]
                                                     - card_winsum[one_player_comb.private_cards.card2]
                                                    ) * win_payoff;
        /*
        if(this->debug) {
            if (one_player_comb.reach_prob_index == 0) {
                cout << (fmt::format("winsum {}",winsum)) << endl;
            }
        }
         */
    }

    // 计算失败时的payoff
    float losssum = 0;
    vector<float>& card_losssum = node->card_sum;
    fill(card_losssum.begin(),card_losssum.end(),0);
    for(int i = 0;i < card_losssum.size();i ++) card_losssum[i] = 0;

    j = oppo_combs.size() - 1;
    for(int i = player_combs.size() - 1;i >= 0;i --){
        const RiverCombs& one_player_comb = player_combs[i];
        while (j >= 0 && one_player_comb.rank > oppo_combs[j].rank){
            const RiverCombs& one_oppo_comb = oppo_combs[j];
            losssum += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            /*
            if(this->debug) {
                if (one_player_comb.reach_prob_index == 0) {
                    cout << (fmt::format("lose :{} "
                            ,this->ranges[oppo][one_oppo_comb.reach_prob_index].weight
                    )) << endl;
                }
            }
            */
            // TODO 这里有问题，要加上reach prob，但是reach prob的index怎么解决？
            card_losssum[one_oppo_comb.private_cards.card1] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            card_losssum[one_oppo_comb.private_cards.card2] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            j --;
        }
        /*
        if(this->debug) {
            cout << (fmt::format("Before Substract {}", payoffs[one_player_comb.reach_prob_index])) << endl;
        }
        */
        payoffs[one_player_comb.reach_prob_index] += (losssum
                                                      - card_losssum[one_player_comb.private_cards.card1]
                                                      - card_losssum[one_player_comb.private_cards.card2]
                                                     ) * lose_payoff;
        /*
        if(this->debug) {
            if (one_player_comb.reach_prob_index == 0) {
                cout << (fmt::format("losssum {}",losssum)) << endl;
            }
        }
         */
    }
    /*
    if(this->debug) {
        cout << endl;
        cout << ("[SHOWDOWN]============") << endl;
        node->printHistory();
        cout << (fmt::format("loss payoffs: {}",lose_payoff));
            player 0 card AdAc
            actions: CALL FOLD
            history: <- (player 1 BET 2.0)
            payoffs : -778.0 -394.0
            regrets: [-192.0, 191.0]
        cout << (fmt::format("oppo sum {}, substracted payoff {}",losssum,payoffs[0]));
    }
    */
    return &payoffs;
}

const vector<float>*
TCfrSolver::terminalUtility(int player, shared_ptr<TerminalNode> node, const vector<vector<float>> &reach_prob, int iter,
                           uint64_t current_board) {
    float player_payoff = node->get_payoffs()[player];

    int oppo = 1 - player;
    const vector<PrivateCards>& player_hand = playerHands(player);
    const vector<PrivateCards>& oppo_hand = playerHands(oppo);

    if(node->utilities.empty()) {
        node->utilities = vector<vector<float>>(2);
    }
    if(node->utilities[player].empty()) {
        node->utilities[player] = vector<float>(this->playerHands(player).size());
    }
    vector<float>& payoffs = node->utilities[player];

    // TODO hard code
    float oppo_sum = 0;
    if(node->oppo_card_sum.empty()) {
        node->oppo_card_sum = vector<float> (52);
    }
    vector<float>& oppo_card_sum = node->oppo_card_sum;
    fill(oppo_card_sum.begin(),oppo_card_sum.end(),0);

    for(int i = 0;i < oppo_hand.size();i ++){
        oppo_card_sum[oppo_hand[i].card1] += reach_prob[oppo][i];
        oppo_card_sum[oppo_hand[i].card2] += reach_prob[oppo][i];
        oppo_sum += reach_prob[oppo][i];
    }

    /*
    if(this->debug) {
        cout << ("[PRETERMINAL]============") << endl;
    }
    */
    for(int i = 0;i < player_hand.size();i ++){
        const PrivateCards& one_player_hand = player_hand[i];
        if(Card::boardsHasIntercept(current_board,Card::boardInts2long(one_player_hand.get_hands()))){
            continue;
        }
        //TODO bug here
        int oppo_same_card_ind = this->pcm.indPlayer2Player(player,oppo,i);
        float plus_reach_prob;
        if(oppo_same_card_ind == -1){
            plus_reach_prob = 0;
        }else{
            plus_reach_prob = reach_prob[oppo][oppo_same_card_ind];
        }
        payoffs[i] = player_payoff * (
                oppo_sum - oppo_card_sum[one_player_hand.card1]
                - oppo_card_sum[one_player_hand.card2]
                + plus_reach_prob
        );
        /*
        if(this->debug) {
            cout << (fmt::format("oppo_card_sum1 {} ", oppo_card_sum[one_player_hand.card1])) << endl;
            cout << (fmt::format("oppo_card_sum2 {} ", oppo_card_sum[one_player_hand.card2])) << endl;
            cout << (fmt::format("reach_prob i {} ", plus_reach_prob)) << endl;
        }
        */
    }

    //TODO 校对图上每个节点payoff
    /*
    if(this->debug) {
        cout << ("[TERMINAL]============") << endl;
        node->printHistory();
        cout << (fmt::format("PPPayoffs: {}",player_payoff)) << endl;
        cout << (fmt::format("reach prob {}",reach_prob[oppo][0])) << endl;
        cout << (fmt::format("oppo sum {}, substracted sum {}",oppo_sum,payoffs[0] / player_payoff)) << endl;
        cout << (fmt::format("substracted sum {}",payoffs[0])) << endl;
    }
    */
    return &payoffs;
}

void TCfrSolver::train() {
    setTrainable(this->tree->getRoot());

    //RiverCombs[][] player_rivers = new RiverCombs[this.player_number][];
    // TODO 回头把BestRespond改完之后回头改掉这一块
    //player_rivers[0] = rrm.getRiverCombos(0, this.range1, board);
    //player_rivers[1] = rrm.getRiverCombos(1, this.range2, board);
    vector<vector<PrivateCards>> player_privates(this->player_number);
    player_privates[0] = pcm.getPreflopCards(0);
    player_privates[1] = pcm.getPreflopCards(1);

    BestResponse br = BestResponse(player_privates,this->player_number,this->pcm,this->rrm,this->deck,this->debug,this->num_threads);

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
            //#pragma omp parallel
            {
                //#pragma omp single
                {
                    cfr(player_id, this->tree->getRoot(), reach_probs, i, this->initial_board_long);
                }
            }
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

template<typename T, typename F, typename Ex>
boost::future<T> TCfrSolver::fork(Ex& ex, F&& func) {
    boost::promise<T> pr;
    boost::future<T> ft = pr.get_future();
    ex.submit_front([p = std::move(pr), f = std::move(func)]() {
        try {
            p.set_value(f());
        } catch (std::exception& e) {
            p.set_exception(e);
        }
    });
    return ft;
}
