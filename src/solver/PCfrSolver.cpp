//
// Created by Xuefeng Huang on 2020/1/31.
//

#include <solver/BestResponse.h>
#include "solver/PCfrSolver.h"

//#define DEBUG;

PCfrSolver::PCfrSolver(shared_ptr<GameTree> tree, vector<PrivateCards> range1, vector<PrivateCards> range2,
                     vector<int> initial_board, shared_ptr<Compairer> compairer, Deck deck, int iteration_number, bool debug,
                     int print_interval, string logfile, string trainer, Solver::MonteCarolAlg monteCarolAlg,int warmup,float accuracy,bool use_isomorphism,int num_threads) :Solver(tree){
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
    this->use_isomorphism = use_isomorphism;

    this->rrm = RiverRangeManager(compairer);
    this->iteration_number = iteration_number;

    vector<vector<PrivateCards>> private_cards(this->player_number);
    private_cards[0] = range1;
    private_cards[1] = range2;
    pcm = PrivateCardsManager(private_cards,this->player_number,Card::boardInts2long(this->initial_board));
    for(int p = 0;p < this->player_number;p ++){
        for(int i = 0;i < this->ranges[p].size();i ++){
            PrivateCards& hand = this->ranges[p][i];
            this->hand_longs[p].push_back(hand.toBoardLong());
            this->hand_card1[p].push_back(hand.card1);
            this->hand_card2[p].push_back(hand.card2);
            this->same_hand[p].push_back(pcm.indPlayer2Player(p,1 - p,i));
        }
    }
    this->debug = debug;
    this->print_interval = print_interval;
    this->monteCarolAlg = monteCarolAlg;
    this->accuracy = accuracy;
    if(num_threads == -1){
        num_threads = omp_get_num_procs();
    }
    cout << fmt::format("Using {} threads",num_threads) << endl;
    this->num_threads = num_threads;
    this->distributing_task = false;
    omp_set_num_threads(this->num_threads);
    setTrainable(this->tree->getRoot());
    this->root_round = this->tree->getRoot()->getRound();
    if(this->root_round == GameTreeNode::GameRound::PREFLOP){
        this->split_round = GameTreeNode::GameRound::FLOP;
    }else if(this->root_round == GameTreeNode::GameRound::FLOP){
        this->split_round = GameTreeNode::GameRound::TURN;
    }else if(this->root_round == GameTreeNode::GameRound::TURN){
        this->split_round = GameTreeNode::GameRound::RIVER;
    }else{
        // do not use multithread in river, really not necessary
        this->split_round = GameTreeNode::GameRound::PREFLOP;
    }
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

vector<float> PCfrSolver::cfr(int player, GameTreeNode* node, const vector<float> &reach_probs, int iter,
                                    uint64_t current_board,int deal) {
    switch(node->getType()) {
        case GameTreeNode::ACTION:
            return actionUtility(player, static_cast<ActionNode*>(node), reach_probs, iter, current_board,deal);
        case GameTreeNode::SHOWDOWN:
            return showdownUtility(player, static_cast<ShowdownNode*>(node), reach_probs, iter, current_board,deal);
        case GameTreeNode::TERMINAL:
            return terminalUtility(player, static_cast<TerminalNode*>(node), reach_probs, iter, current_board,deal);
        case GameTreeNode::CHANCE:
            return chanceUtility(player, static_cast<ChanceNode*>(node), reach_probs, iter, current_board,deal);
        default:
            throw runtime_error("node type unknown");
    }
}

vector<float>
PCfrSolver::chanceUtility(int player, ChanceNode* node, const vector<float> &reach_probs, int iter,
                         uint64_t current_board,int deal) {
    vector<Card>& cards = this->deck.getCards();
    //float[] cardWeights = getCardsWeights(player,reach_probs[1 - player],current_board);

    int card_num = node->getCards().size();
    if(card_num % 4 != 0) throw runtime_error("card num cannot round 4");
    // 可能的发牌情况,2代表每个人的holecard是两张
    int possible_deals = node->getCards().size() - Card::long2board(current_board).size() - 2;
    int oppo = 1 - player;

    //vector<float> chance_utility(reach_probs[player].size());
    vector<float> chance_utility = vector<float>(this->ranges[player].size());
    fill(chance_utility.begin(),chance_utility.end(),0);

    int random_deal = 0;
    if(this->monteCarolAlg==MonteCarolAlg::PUBLIC) {
        if (this->round_deal[GameTreeNode::gameRound2int(node->getRound())] == -1) {
            random_deal = random(1, possible_deals + 1 + 2);
            this->round_deal[GameTreeNode::gameRound2int(node->getRound())] = random_deal;
        } else {
            random_deal = this->round_deal[GameTreeNode::gameRound2int(node->getRound())];
        }
    }
    //vector<vector<vector<float>>> arr_new_reach_probs = vector<vector<vector<float>>>(node->getCards().size());

    vector<vector<float>> results(node->getCards().size());
    //fill(results.begin(),results.end(),nullptr);

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

    vector<int> valid_cards;
    valid_cards.reserve(node->getCards().size());

    for(int card = 0;card < node->getCards().size();card ++) {
        Card *one_card = const_cast<Card *>(&(node->getCards()[card]));
        uint64_t card_long = Card::boardInt2long(one_card->getCardInt());//Card::boardCards2long(new Card[]{one_card});
        if (Card::boardsHasIntercept(card_long, current_board)) continue;
        if (iter <= this->warmup && multiplier[card] == 0) continue;
        if (this->color_iso_offset[deal][one_card->getCardInt() % 4] < 0) continue;
        valid_cards.push_back(card);
    }

    #pragma omp parallel for schedule(dynamic,1)
    for(int valid_ind = 0;valid_ind < valid_cards.size();valid_ind++) {
        int card = valid_cards[valid_ind];
        GameTreeNode* one_child = node->getChildRaw();
        Card *one_card = const_cast<Card *>(&(node->getCards()[card]));
        uint64_t card_long = Card::boardInt2long(one_card->getCardInt());//Card::boardCards2long(new Card[]{one_card});

        uint64_t new_board_long = current_board | card_long;
        if (this->monteCarolAlg == MonteCarolAlg::PUBLIC) {
            throw runtime_error("parallel solver don't support public monte carol");
        }

        //cout << "Card deal:" << one_card->toString() << endl;

        vector<PrivateCards> &playerPrivateCard = (this->ranges[player]);
        vector<PrivateCards> &oppoPrivateCards = (this->ranges[1 - player]);


        vector<float> new_reach_probs = vector<float>(oppoPrivateCards.size());



#ifdef DEBUG
        if (playerPrivateCard.size() != this->ranges[player].size()) throw runtime_error("length not match");
        if (oppoPrivateCards.size() != this->ranges[1 - player].size()) throw runtime_error("length not match");
#endif

        int player_hand_len = this->ranges[oppo].size();
        const uint64_t* oppo_longs = this->hand_longs[oppo].data();
        for (int player_hand = 0; player_hand < player_hand_len; player_hand++) {
            if (Card::boardsHasIntercept(card_long, oppo_longs[player_hand])) {
                new_reach_probs[player_hand] = 0;
                continue;
            }
            new_reach_probs[player_hand] = reach_probs[player_hand] / possible_deals;
        }
#ifdef DEBUG
        if (Card::boardsHasIntercept(current_board, card_long))
            throw runtime_error("board has intercept with dealt card");
#endif

        int new_deal;
        if(deal == 0){
            new_deal = card + 1;
        } else if (deal > 0 && deal <= card_num){
            int origin_deal = deal - 1;

#ifdef DEBUG
            if(origin_deal == card) throw runtime_error("deal should not be equal");
#endif
            new_deal = card_num * origin_deal + card;
            new_deal += (1 + card_num);
        } else{
            throw runtime_error(fmt::format("deal out of range : {} ",deal));
        }
        if(this->distributing_task && node->getRound() == this->split_round) {
            results[one_card->getNumberInDeckInt()] = vector<float>(this->ranges[player].size());
            //TaskParams taskParams = TaskParams();
        }else {
            vector<float> child_utility = this->cfr(player, one_child, new_reach_probs, iter, new_board_long, new_deal);
            results[one_card->getNumberInDeckInt()] = child_utility;
        }
    }

    for(int card = 0;card < node->getCards().size();card ++) {
        Card *one_card = const_cast<Card *>(&(node->getCards()[card]));
        vector<float> exchanged;
        const vector<float>* child_utility_ptr;
        int offset = this->color_iso_offset[deal][one_card->getCardInt() % 4];
        if(offset < 0) {
            int rank1 = one_card->getCardInt() % 4;
            int rank2 = rank1 + offset;
            exchanged = results[one_card->getNumberInDeckInt() + offset];
            exchange_color(exchanged,this->pcm.getPreflopCards(player),rank1,rank2);
            child_utility_ptr = &exchanged;
        }else{
            child_utility_ptr = &results[one_card->getNumberInDeckInt()];
        }
        const vector<float>& child_utility = *child_utility_ptr;
        if(child_utility.empty())
            continue;

        if(iter > this->warmup) {
            for (int i = 0; i < child_utility.size(); i++)
                chance_utility[i] += child_utility[i];
        }else{
            for (int i = 0; i < child_utility.size(); i++)
                chance_utility[i] += child_utility[i] * multiplier[card];
        }
    }

#ifdef DEBUG
    if(this->monteCarolAlg == MonteCarolAlg::PUBLIC) {
        throw runtime_error("not possible");
    }
    if(chance_utility.size() != this->ranges[player].size()) {
        throw runtime_error("size problems");
    }
#endif
    return chance_utility;
}

vector<float>
PCfrSolver::actionUtility(int player, ActionNode* node, const vector<float> &reach_probs, int iter,
                         uint64_t current_board,int deal) {
    const int node_player = node->getPlayer();
    const int hands = this->ranges[node_player].size();
    const int player_hands = this->ranges[player].size();

    vector<float> payoffs(player_hands);
    vector<shared_ptr<GameTreeNode>>& children =  node->getChildrens();
    const int action_number = node->getActions().size();

    Trainable* trainable = node->getTrainableRaw(deal);
    const vector<float> current_strategy = trainable->getcurrentStrategy();
    const float* strategy = current_strategy.data();

    vector<vector<float>> results(action_number);
    for (int action_id = 0; action_id < action_number; action_id++) {
        if (node_player != player) {
            vector<float> new_reach_prob(reach_probs.size());
            const float* reach = reach_probs.data();
            const float* action_strategy = strategy + action_id * hands;
            float* out = new_reach_prob.data();
            const int reach_len = new_reach_prob.size();
            for (int hand_id = 0; hand_id < reach_len; hand_id++) {
                out[hand_id] = reach[hand_id] * action_strategy[hand_id];
            }
            results[action_id] = this->cfr(player, children[action_id].get(), new_reach_prob, iter, current_board,deal);
        }else {
            results[action_id] = this->cfr(player, children[action_id].get(), reach_probs, iter, current_board,deal);
        }
    }

    for (int action_id = 0; action_id < action_number; action_id++) {
        const vector<float>& action_utilities = results[action_id];
        if(action_utilities.empty()){
            continue;
        }
        const float* utility = action_utilities.data();
        float* payoff = payoffs.data();
        const int len = action_utilities.size();
        if (player == node_player) {
            const float* action_strategy = strategy + action_id * hands;
            for (int hand_id = 0; hand_id < len; hand_id++) {
                payoff[hand_id] += action_strategy[hand_id] * utility[hand_id];
            }
        } else {
            for (int hand_id = 0; hand_id < len; hand_id++) {
                payoff[hand_id] += utility[hand_id];
            }
        }
    }

    if (player == node_player) {
        //为了节省计算成本将action regret 存在一位数组而不是二维数组中，两个纬度分别是（该infoset有多少动作,该palyer有多少holecard）
        vector<float> regrets(action_number * hands);
        for (int action_id = 0; action_id < action_number; action_id++) {
            const float* utility = results[action_id].data();
            const float* payoff = payoffs.data();
            float* action_regret = regrets.data() + action_id * hands;
            for (int i = 0; i < hands; i++) {
                action_regret[i] = utility[i] - payoff[i];
            }
        }

        if(!this->distributing_task) {
            if (iter > this->warmup) {
                trainable->updateRegrets(regrets, iter + 1, reach_probs);
            }
            else {
                // iter == this->warmup
                vector<int> deals = this->getAllAbstractionDeal(deal);
                shared_ptr<Trainable> standard_trainable = nullptr;
                for (int one_deal : deals) {
                    shared_ptr<Trainable> one_trainable = node->getTrainable(one_deal);
                    if (standard_trainable == nullptr) {
                        one_trainable->updateRegrets(regrets, iter + 1, reach_probs);
                        standard_trainable = one_trainable;
                    } else {
                        one_trainable->copyStrategy(standard_trainable);
                    }
                }
            }
        }
    }
    return payoffs;

}

vector<float>
PCfrSolver::showdownUtility(int player, ShowdownNode* node, const vector<float> &reach_probs,
                           int iter, uint64_t current_board,int deal) {
    // player win时候player的收益，player lose的时候收益明显为-player_payoff
    int oppo = 1 - player;
    float win_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE,player,player);
    float lose_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE,oppo,player);
    const vector<PrivateCards>& player_private_cards = this->ranges[player];
    const vector<PrivateCards>& oppo_private_cards = this->ranges[oppo];

    const vector<RiverCombs>& player_combs = this->rrm.getRiverCombos(player,player_private_cards,current_board);
    const vector<RiverCombs>& oppo_combs = this->rrm.getRiverCombos(oppo,oppo_private_cards,current_board);

    vector<float> payoffs = vector<float>(player_private_cards.size());

    float winsum = 0;
    float card_winsum[52] = {0};

    int j = 0;
    for(int i = 0;i < player_combs.size();i ++){
        const RiverCombs& one_player_comb = player_combs[i];
        while (j < oppo_combs.size() && one_player_comb.rank < oppo_combs[j].rank){
            const RiverCombs& one_oppo_comb = oppo_combs[j];
            winsum += reach_probs[one_oppo_comb.reach_prob_index];
            card_winsum[one_oppo_comb.card1] += reach_probs[one_oppo_comb.reach_prob_index];
            card_winsum[one_oppo_comb.card2] += reach_probs[one_oppo_comb.reach_prob_index];
            j ++;
        }
        payoffs[one_player_comb.reach_prob_index] = (winsum
                                                     - card_winsum[one_player_comb.card1]
                                                     - card_winsum[one_player_comb.card2]
                                                    ) * win_payoff;
    }

    // 计算失败时的payoff
    float losssum = 0;
    float (&card_losssum)[52] = card_winsum;
    fill(card_losssum,card_losssum + 52,0);

    j = oppo_combs.size() - 1;
    for(int i = player_combs.size() - 1;i >= 0;i --){
        const RiverCombs& one_player_comb = player_combs[i];
        while (j >= 0 && one_player_comb.rank > oppo_combs[j].rank){
            const RiverCombs& one_oppo_comb = oppo_combs[j];
            losssum += reach_probs[one_oppo_comb.reach_prob_index];
            card_losssum[one_oppo_comb.card1] += reach_probs[one_oppo_comb.reach_prob_index];
            card_losssum[one_oppo_comb.card2] += reach_probs[one_oppo_comb.reach_prob_index];
            j --;
        }
        payoffs[one_player_comb.reach_prob_index] += (losssum
                                                      - card_losssum[one_player_comb.card1]
                                                      - card_losssum[one_player_comb.card2]
                                                     ) * lose_payoff;
    }
    return payoffs;
}

vector<float>
PCfrSolver::terminalUtility(int player, TerminalNode* node, const vector<float> &reach_prob, int iter,
                           uint64_t current_board,int deal) {
    float player_payoff = node->payoffOf(player);

    int oppo = 1 - player;
    const int player_len = this->hand_longs[player].size();
    const int oppo_len = this->hand_longs[oppo].size();
    const int* oppo_card1 = this->hand_card1[oppo].data();
    const int* oppo_card2 = this->hand_card2[oppo].data();
    const int* player_card1 = this->hand_card1[player].data();
    const int* player_card2 = this->hand_card2[player].data();
    const uint64_t* player_longs = this->hand_longs[player].data();
    const int* same = this->same_hand[player].data();
    const float* reach = reach_prob.data();

    vector<float> payoffs(player_len);

    float oppo_sum = 0;
    float oppo_card_sum[52] = {0};

    for(int i = 0;i < oppo_len;i ++){
        oppo_card_sum[oppo_card1[i]] += reach[i];
        oppo_card_sum[oppo_card2[i]] += reach[i];
        oppo_sum += reach[i];
    }

    for(int i = 0;i < player_len;i ++){
        if(Card::boardsHasIntercept(current_board,player_longs[i])){
            continue;
        }
        int oppo_same_card_ind = same[i];
        float plus_reach_prob = oppo_same_card_ind == -1 ? 0 : reach[oppo_same_card_ind];
        payoffs[i] = player_payoff * (
                oppo_sum - oppo_card_sum[player_card1[i]]
                - oppo_card_sum[player_card2[i]]
                + plus_reach_prob
        );
    }

    return payoffs;
}

void PCfrSolver::findGameSpecificIsomorphisms() {
    // hand isomorphisms
    vector<Card> board_cards = Card::long2boardCards(this->initial_board_long);
    for(int i = 0;i <= 1;i ++){
        vector<PrivateCards>& range = i == 0?this->range1:this->range2;
        for(int i_range = 0;i_range < range.size();i_range ++) {
            PrivateCards one_range = range[i_range];
            uint32_t range_hash[4]; // four colors, hash of the isomorphisms range + hand combos
            for(int i = 0;i < 4;i ++)range_hash[i] = 0;
            for (int color = 0; color < 4; color++) {
                for (Card one_card:board_cards) {
                    if (one_card.getCardInt() % 4 == color) {
                        range_hash[color] = range_hash[color] | (1 << (one_card.getCardInt() / 4));
                    }
                }
            }
            for (int color = 0; color < 4; color++) {
                for (int one_card_int:{one_range.card1,one_range.card2}) {
                    if (one_card_int % 4 == color) {
                        range_hash[color] = range_hash[color] | (1 << (one_card_int / 4 + 16));
                    }
                }
            }
            // TODO check whethe hash is equal with others
        }
    }

    // chance node isomorphisms
    uint16_t color_hash[4];
    for(int i = 0;i < 4;i ++)color_hash[i] = 0;
    for (Card one_card:board_cards) {
        int rankind = one_card.getCardInt() % 4;
        int suitind = one_card.getCardInt() / 4;
        color_hash[rankind] = color_hash[rankind] | (1 << suitind);
    }
    for(int i = 0;i < 4;i ++){
        this->color_iso_offset[0][i] = 0;
        for(int j = 0;j < i;j ++){
            if(color_hash[i] == color_hash[j]){
                this->color_iso_offset[0][i] = j - i;
                continue;
            }
        }
    }
    for(int deal = 0;deal < this->deck.getCards().size();deal ++) {
        uint16_t color_hash[4];
        for(int i = 0;i < 4;i ++)color_hash[i] = 0;
        // chance node isomorphisms
        for (Card one_card:board_cards) {
            int rankind = one_card.getCardInt() % 4;
            int suitind = one_card.getCardInt() / 4;
            color_hash[rankind] = color_hash[rankind] | (1 << suitind);
        }
        Card one_card = this->deck.getCards()[deal];
        int rankind = one_card.getCardInt() % 4;
        int suitind = one_card.getCardInt() / 4;
        color_hash[rankind] = color_hash[rankind] | (1 << suitind);
        for (int i = 0; i < 4; i++) {
            this->color_iso_offset[deal + 1][i] = 0;
            for (int j = 0; j < i; j++) {
                if (color_hash[i] == color_hash[j]) {
                    this->color_iso_offset[deal + 1][i] = j - i;
                    continue;
                }
            }
        }
    }
}

void PCfrSolver::purnTree() {
    // TODO how to purn the tree, use wramup to start training in memory-save mode, and switch to purn tree directly to both save memory and speedup
}

void PCfrSolver::train() {

    vector<vector<PrivateCards>> player_privates(this->player_number);
    player_privates[0] = pcm.getPreflopCards(0);
    player_privates[1] = pcm.getPreflopCards(1);
    if(this->use_isomorphism){
        this->findGameSpecificIsomorphisms();
    }

    BestResponse br = BestResponse(player_privates,this->player_number,this->pcm,this->rrm,this->deck,this->debug,this->color_iso_offset,this->split_round,this->num_threads);

    br.printExploitability(tree->getRoot(), 0, tree->getRoot()->getPot(), initial_board_long);

    vector<vector<float>> reach_probs = this->getReachProbs();
    ofstream fileWriter;
    if(!this->logfile.empty())fileWriter.open(this->logfile);

    uint64_t begintime = timeSinceEpochMillisec();
    uint64_t endtime = timeSinceEpochMillisec();

    for(int i = 0;i < this->iteration_number;i++){
        for(int player_id = 0;player_id < this->player_number;player_id ++) {
            this->round_deal = vector<int>{-1,-1,-1,-1};
            //#pragma omp parallel
            {
                //#pragma omp single
                {
                    //this->distributing_task = true;
                    cfr(player_id, this->tree->getRoot().get(), reach_probs[1 - player_id], i, this->initial_board_long,0);
                    //throw runtime_error("returning...");
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
            if(expliotibility <= this->accuracy){
                break;
            }
            //begintime = timeSinceEpochMillisec();
        }
    }
    if(!this->logfile.empty()) {
        fileWriter.flush();
        fileWriter.close();
    }

}

void PCfrSolver::exchangeRange(json& strategy,int rank1,int rank2,shared_ptr<ActionNode> one_node){
    if(rank1 == rank2)return;
    int player = one_node->getPlayer();
    vector<string> range_strs;
    vector<vector<float>> strategies;

    for(int i = 0;i < this->ranges[player].size();i ++){
        string one_range_str = this->ranges[player][i].toString();
        if(!strategy.contains(one_range_str)){
            for(auto one_key:strategy.items()){
                cout << one_key.key() << endl;
            }
            cout << "strategy: " << strategy  << endl;
            throw runtime_error(fmt::format("{} not exist in strategy",one_range_str));
        }
        vector<float> one_strategy = strategy[one_range_str];
        range_strs.push_back(one_range_str);
        strategies.push_back(one_strategy);
    }
    exchange_color(strategies,this->ranges[player],rank1,rank2);

    for(int i = 0;i < this->ranges[player].size();i ++) {
        string one_range_str = this->ranges[player][i].toString();
        vector<float> one_strategy = strategies[i];
        strategy[one_range_str] = one_strategy;
    }
}

void PCfrSolver::filterColliding(json& strategy_block,uint64_t current_board_long) {
    if(!strategy_block.contains("strategy")) return;
    json& combo_map = strategy_block["strategy"];
    if(!combo_map.is_object()) return;
    vector<string> to_drop;
    for(auto it = combo_map.begin();it != combo_map.end();++ it){
        const string& combo = it.key();
        if(combo.size() < 4) continue;
        vector<string> hole{combo.substr(0,2),combo.substr(2,2)};
        uint64_t hole_long = Card::boardCards2long(hole);
        if(Card::boardsHasIntercept(hole_long,current_board_long)){
            to_drop.push_back(combo);
        }
    }
    for(const string& combo : to_drop) combo_map.erase(combo);
}

void PCfrSolver::reConvertJson(const shared_ptr<GameTreeNode>& node,json& strategy,string key,int depth,int max_depth,vector<string> prefix,int deal,vector<vector<int>> exchange_color_list,uint64_t current_board_long) {
    if(depth >= max_depth) return;
    if(node->getType() == GameTreeNode::GameTreeNodeType::ACTION) {
        json* retval;
        if(key != ""){
            strategy[key] = json();
            retval = &(strategy[key]);
        }else{
            retval = &strategy;
        }

        shared_ptr<ActionNode> one_node = std::dynamic_pointer_cast<ActionNode>(node);

        vector<string> actions_str;
        for(GameActions one_action:one_node->getActions()) actions_str.push_back(one_action.toString());

        (*retval)["actions"] = actions_str;
        (*retval)["player"] = one_node->getPlayer();

        (*retval)["childrens"] = json();
        json& childrens = (*retval)["childrens"];

        for(int i = 0;i < one_node->getActions().size();i ++){
            GameActions& one_action = one_node->getActions()[i];
            shared_ptr<GameTreeNode> one_child = one_node->getChildrens()[i];
            vector<string> new_prefix(prefix);
            new_prefix.push_back(one_action.toString());
            this->reConvertJson(one_child,childrens,one_action.toString(),depth,max_depth,new_prefix,deal,exchange_color_list,current_board_long);
        }
        if((*retval)["childrens"].empty()){
            (*retval).erase("childrens");
        }
        shared_ptr<Trainable> trainable = one_node->getTrainable(deal,false);
        if(trainable != nullptr) {
            (*retval)["strategy"] = trainable->dump_strategy(false);
            for(vector<int> one_exchange:exchange_color_list){
                int rank1 = one_exchange[0];
                int rank2 = one_exchange[1];
                this->exchangeRange((*retval)["strategy"]["strategy"],rank1,rank2,one_node);

            }
            PCfrSolver::filterColliding((*retval)["strategy"],current_board_long);
        }
        (*retval)["node_type"] = "action_node";

    }else if(node->getType() == GameTreeNode::GameTreeNodeType::SHOWDOWN) {
    }else if(node->getType() == GameTreeNode::GameTreeNodeType::TERMINAL) {
    }else if(node->getType() == GameTreeNode::GameTreeNodeType::CHANCE) {
        json* retval;
        if(key != ""){
            strategy[key] = json();
            retval = &(strategy[key]);
        }else{
            retval = &strategy;
        }

        shared_ptr<ChanceNode> chanceNode = std::dynamic_pointer_cast<ChanceNode>(node);
        const vector<Card>& cards = chanceNode->getCards();
        shared_ptr<GameTreeNode> childerns = chanceNode->getChildren();
        vector<string> card_strs;
        for(Card card:cards)
            card_strs.push_back(card.toString());

        json& dealcards = (*retval)["dealcards"];
        for(int i = 0;i < cards.size();i ++){
            vector<vector<int>> new_exchange_color_list(exchange_color_list);
            Card& one_card = const_cast<Card &>(cards[i]);
            vector<string> new_prefix(prefix);
            new_prefix.push_back("Chance:" + one_card.toString());

            int card = i;

            int offset = this->color_iso_offset[deal][one_card.getCardInt() % 4];
            if(offset < 0) {
                for(int x = 0;x < cards.size();x ++){
                    if(
                            Card::card2int(cards[x]) ==
                            (Card::card2int(cards[card]) + offset)
                    ){
                        card = x;
                        break;
                    }
                }
                if(card == i){
                    throw runtime_error("isomorphism not found while dump strategy");
                }
                vector<int> one_exchange{one_card.getCardInt() % 4,one_card.getCardInt() % 4 + offset};
                new_exchange_color_list.push_back(one_exchange);
            }

            int card_num = this->deck.getCards().size();
            int new_deal;
            if(deal == 0){
                new_deal = card + 1;
            } else if (deal > 0 && deal <= card_num){
                int origin_deal = deal - 1;

#ifdef DEBUG
                if(origin_deal == card) throw runtime_error("deal should not be equal");
#endif
                new_deal = card_num * origin_deal + card;
                new_deal += (1 + card_num);
            } else{
                throw runtime_error(fmt::format("deal out of range : {} ",deal));
            }

            if(exchange_color_list.size() > 1){
                throw runtime_error("exchange color list shouldn't be exceed size 1 here");
            }

            string one_card_str = one_card.toString();
            if(exchange_color_list.size() == 1) {
                int rank1 = exchange_color_list[0][0];
                int rank2 = exchange_color_list[0][1];
                if(one_card.getCardInt() % 4 == rank1){
                    one_card_str = Card::intCard2Str(one_card.getCardInt() - rank1 + rank2);
                }else if(one_card.getCardInt() % 4 == rank2){
                    one_card_str = Card::intCard2Str(one_card.getCardInt() - rank2 + rank1);
                }

            }

            // Extend the board mask with the chance card actually written into the
            // JSON (after color isomorphism). Subtrees rooted here will use this to
            // drop combos that physically collide with the runout.
            uint64_t child_board_long = current_board_long | Card::boardCards2long(vector<string>{one_card_str});

            this->reConvertJson(childerns,dealcards,one_card_str,depth + 1,max_depth,new_prefix,new_deal,new_exchange_color_list,child_board_long);
        }
        if((*retval)["dealcards"].empty()){
            (*retval).erase("dealcards");
        }

        (*retval)["deal_number"] = dealcards.size();
        (*retval)["node_type"] = "chance_node";
    }else{
        throw runtime_error("node type unknown!!");
    }
}

json PCfrSolver::dumps(bool with_status,int depth) {
    if(with_status == true){
        throw runtime_error("");
    }
    json retjson;
    this->reConvertJson(this->tree->getRoot(),retjson,"",0,depth,vector<string>({"begin"}),0,vector<vector<int>>(),this->initial_board_long);
    return std::move(retjson);
}

