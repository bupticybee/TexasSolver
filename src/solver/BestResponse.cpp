//
// Created by Xuefeng Huang on 2020/2/1.
//

#include "include/solver/BestResponse.h"
#include <QtCore>
#include <QObject>
#include <QTranslator>
//#define DEBUG;

BestResponse::BestResponse(vector<vector<PrivateCards>> &private_combos, int player_number,
                           PrivateCardsManager &pcm, RiverRangeManager &rrm, Deck &deck, bool debug,int color_iso_offset[][4],GameTreeNode::GameRound split_round,int nthreads, int use_halffloats)
                           :rrm(rrm),pcm(pcm),private_combos(private_combos),deck(deck){
    this->player_number = player_number;
    this->debug = debug;

#ifdef DEBUG
    if(private_combos.size() != player_number)
        throw runtime_error(
                tfm::format("river combo length NE player nunber: %s -- %s",private_combos.size(),player_number)
        );
#endif
    player_hands = vector<int>(player_number);
    for(int i = 0;i < player_number;i ++) {
        player_hands[i] = private_combos[i].size();
    }
    this->nthreads = nthreads;
    this->use_halffloats = use_halffloats;
    for(int i = 0;i < 52 * 52 * 2;i ++){
        for(int j = 0;j < 4;j ++){
            this->color_iso_offset[i][j] = color_iso_offset[i][j];
        }
    }
    this->split_round = split_round;
    omp_set_num_threads(this->nthreads);
}

float BestResponse::printExploitability(shared_ptr<GameTreeNode> root, int iterationCount, float initial_pot,
                                        uint64_t initialBoard) {
    if(this->reach_probs.empty())
        this->reach_probs = vector<vector<float>> (this->player_number);

    qDebug().noquote() << QString::fromStdString(tfm::format(QObject::tr("Iter: %s").toStdString().c_str(),iterationCount));
    float exploitible = 0;
    // 构造双方初始reach probs(按照手牌weights)
    for (int player_id = 0; player_id < this->player_number; player_id++) {
        if(reach_probs[player_id].empty()) {
            reach_probs[player_id] = vector<float>(private_combos[player_id].size());
        }
        for (std::size_t hc = 0; hc < private_combos[player_id].size(); hc++)
            reach_probs[player_id][hc] = private_combos[player_id][hc].weight;
    }

    for (int player_id = 0; player_id < this->player_number; player_id++) {
        float player_exploitability = getBestReponseEv(root, player_id, reach_probs, initialBoard, 0);
        exploitible += player_exploitability;
        qDebug().noquote() << (QString::fromStdString(tfm::format(QObject::tr("player %s exploitability %s").toStdString().c_str(), player_id, player_exploitability)));
    }
    float total_exploitability = exploitible / this->player_number / initial_pot * 100;
    qDebug().noquote() << QString::fromStdString(tfm::format(QObject::tr("Total exploitability %s precent").toStdString().c_str(), total_exploitability));
    return total_exploitability;
}

float BestResponse::getBestReponseEv(shared_ptr<GameTreeNode> node, int player, vector<vector<float>> reach_probs,
                                     uint64_t initialBoard, int deal) {
    float ev = 0;
    //考虑（1）相对的手牌 proability,(2)被场面和对手ban掉的手牌
    const vector<float>& private_cards_evs = bestResponse(node, player, reach_probs, initialBoard, deal);
    vector<PrivateCards>& player_combo = this->private_combos[player];
    vector<PrivateCards>& oppo_combo = this->private_combos[1 - player];

    for(std::size_t player_hand = 0;player_hand < player_combo.size();player_hand ++){
        float one_payoff = private_cards_evs[player_hand];
        PrivateCards& one_player_hand = (player_combo)[player_hand];
        uint64_t private_long = one_player_hand.toBoardLong();
        if(Card::boardsHasIntercept(private_long,initialBoard)){
            continue;
        }
        float oppo_sum = 0;

        for(std::size_t oppo_hand = 0;oppo_hand < oppo_combo.size();oppo_hand ++){
            PrivateCards& one_oppo_hand = (oppo_combo)[oppo_hand];
            uint64_t private_long_oppo = one_oppo_hand.toBoardLong();
            if(Card::boardsHasIntercept(private_long,private_long_oppo)
               || Card::boardsHasIntercept(private_long_oppo,initialBoard)){
                continue;
            }
            oppo_sum += one_oppo_hand.weight;
        }
        ev +=  one_payoff * one_player_hand.relative_prob / oppo_sum;

    }

    return ev;
}

vector<float> BestResponse::bestResponse(shared_ptr<GameTreeNode> node, int player,const vector<vector<float>>& reach_probs,
                                         uint64_t board,int deal) {
    if (node->getType() == GameTreeNode::ACTION) {
        shared_ptr<ActionNode> action_node = std::dynamic_pointer_cast<ActionNode>(node);
        return actionBestResponse(action_node, player, reach_probs, board, deal);
    } else if (node->getType() == GameTreeNode::SHOWDOWN) {
        shared_ptr<ShowdownNode> showdown_node = std::dynamic_pointer_cast<ShowdownNode>(node);
        return showdownBestResponse(showdown_node, player, reach_probs, board, deal);
    } else if (node->getType() == GameTreeNode::TERMINAL) {
        shared_ptr<TerminalNode> terminal_node = std::dynamic_pointer_cast<TerminalNode>(node);
        return terminalBestReponse(terminal_node, player, reach_probs, board, deal);
    } else if (node->getType() == GameTreeNode::CHANCE) {
        shared_ptr<ChanceNode> chance_node = std::dynamic_pointer_cast<ChanceNode>(node);
        return chanceBestReponse(chance_node, player, reach_probs, board, deal);
    }else
        throw runtime_error("Node type not understood ");
}

vector<float>
BestResponse::chanceBestReponse(shared_ptr<ChanceNode> node, int player,const vector<vector<float>>& reach_probs,
                                uint64_t current_board, int deal) {
    vector<Card>& cards = this->deck.getCards();

    int card_num = node->getCards().size();
    // 可能的发牌情况,2代表每个人的holecard是两张
    int possible_deals = node->getCards().size() - Card::long2board(current_board).size() - 2;

    vector<float> chance_utility = vector<float>(reach_probs[player].size());
    fill(chance_utility.begin(),chance_utility.end(),0);

    vector<vector<vector<float>>> best_respond_arr_new_reach_probs = vector<vector<vector<float>>>(node->getCards().size());
    // 遍历每一种发牌的可能性

    vector<vector<float>> results(node->getCards().size());

    #pragma omp parallel for
    for(std::size_t card = 0;card < node->getCards().size();card ++) {
        shared_ptr<GameTreeNode> one_child = node->getChildren();
        Card one_card = node->getCards()[card];
        uint64_t card_long = Card::boardInt2long(one_card.getCardInt());

        // 不可能发出和board重复的牌，对吧
        if (Card::boardsHasIntercept(card_long, current_board)) continue;
        if(this->color_iso_offset[deal][one_card.getCardInt() % 4] < 0) continue;

        const vector<PrivateCards> &playerPrivateCard = this->pcm.getPreflopCards(
                player);//this.getPlayerPrivateCard(player);
        const vector<PrivateCards> &oppoPrivateCards = this->pcm.getPreflopCards(1 - player);

        if (best_respond_arr_new_reach_probs[card].empty()) {
            best_respond_arr_new_reach_probs[card] = vector<vector<float>>(2);
        }
        vector<vector<float>> new_reach_probs = best_respond_arr_new_reach_probs[card];
        if (new_reach_probs[player].empty()) {
            new_reach_probs[player] = vector<float>(playerPrivateCard.size());
            new_reach_probs[1 - player] = vector<float>(oppoPrivateCards.size());
        }

        //new_reach_probs[player] = new float[playerPrivateCard.length];

#ifdef DEBUG
        if (reach_probs[player].size() != playerPrivateCard.size())
            throw runtime_error("length mismatch");

        // 检查是否双方 hand和reach prob长度符合要求
        if (playerPrivateCard.size() != reach_probs[player].size()) throw runtime_error("length not match1 ");
        if (oppoPrivateCards.size() != reach_probs[1 - player].size()) throw runtime_error("length not match2 ");
#endif

        for (int one_player = 0; one_player < 2; one_player++) {
            int player_hand_len = this->pcm.getPreflopCards(one_player).size();
            for (int player_hand = 0; player_hand < player_hand_len; player_hand++) {
                uint64_t privateBoardLong = this->pcm.getPreflopCards(one_player)[player_hand].toBoardLong();
                if (Card::boardsHasIntercept(card_long, privateBoardLong)) {
                    new_reach_probs[one_player][player_hand] = 0;
                    continue;
                }
                new_reach_probs[one_player][player_hand] = reach_probs[one_player][player_hand] / possible_deals;
            }
        }

#ifdef DEBUG
        if (Card::boardsHasIntercept(current_board, card_long))
            throw runtime_error("board has intercept with dealt card");
#endif
        uint64_t new_board_long = current_board | card_long;

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
            throw runtime_error(tfm::format("deal out of range : %s ",deal));
        }
        vector<float> child_utility = this->bestResponse(one_child, player, new_reach_probs, new_board_long,new_deal);
        results[one_card.getNumberInDeckInt()] = child_utility;
    }

    for(std::size_t card = 0;card < node->getCards().size();card ++) {
        Card *one_card = const_cast<Card *>(&(node->getCards()[card]));
        vector<float> child_utility;
        int offset = this->color_iso_offset[deal][one_card->getCardInt() % 4];
        if(offset < 0) {
            int rank1 = one_card->getCardInt() % 4;
            int rank2 = rank1 + offset;
#ifdef DEBUG
            if(rank2 < 0) throw runtime_error("rank error");
#endif
            // TODO 这里需要调换一下颜色,根据offset
            child_utility = results[one_card->getNumberInDeckInt() + offset];
            exchange_color(child_utility,private_combos[player],rank1,rank2);
        }else{
            child_utility = results[one_card->getNumberInDeckInt()];
        }

        if(child_utility.empty())
            continue;
#ifdef DEBUG
        if(child_utility.size() != chance_utility.size()) throw runtime_error("length not match3 ");
#endif
        for(std::size_t i = 0;i < child_utility.size();i ++)
            chance_utility[i] += (child_utility)[i];
    }

    return chance_utility;
}

vector<float>
BestResponse::actionBestResponse(shared_ptr<ActionNode> node, int player, const vector<vector<float>>& reach_probs,
                                 uint64_t board, int deal) {
    if(player == node->getPlayer()){
        // 如果是自己在做决定，那么肯定选对自己的最有利的，反之对于对方来说，这个就是我方expliot了对方,
        // 这里可以当成"player"做决定的时候，action prob是0-1分布，因为需要使用最好的策略去expliot对方，最好的策略一定是ont-hot的
        vector<float> my_exploitability = vector<float>(reach_probs[player].size());

        bool first_action_flag = true;
        for(shared_ptr<GameTreeNode> one_node:node->getChildrens()){
            const vector<float>& node_ev = this->bestResponse(one_node,player,reach_probs,board,deal);
            if(first_action_flag){
                my_exploitability.assign(node_ev.begin(),node_ev.end());
                first_action_flag = false;
            }else {
                for (std::size_t i = 0;i < node_ev.size();i ++) {
                    my_exploitability[i] = max(my_exploitability[i],node_ev[i]);
                }
            }
        }
        if(this->debug) {
            cout << ("[action]") << endl;
            node->printHistory();
            //cout << (Arrays.toString(my_exploitability));
        }
        return my_exploitability;
    }else{
        // 如果是别人做决定，那么就按照别人的策略加权算出一个 ev
        vector<float> total_payoffs = vector<float>(player_hands[player]);
        fill(total_payoffs.begin(),total_payoffs.end(),0);
        shared_ptr<Trainable> trainable = node->getTrainable(deal,true,this->use_halffloats);
#ifdef DEBUG
        if(trainable == nullptr){
            throw runtime_error("null trainable");
        }
#endif
        const vector<float>& node_strategy = trainable->getAverageStrategy();
#ifdef DEBUG
        if(node_strategy.size() != node->getChildrens().size() * reach_probs[node->getPlayer()].size()) {
            throw runtime_error(tfm::format("strategy size not match %s - %s",
                                                     node_strategy.size(), node->getChildrens().size() * reach_probs[node->getPlayer()].size()));
        }
#endif

        vector<vector<vector<float>>> best_respond_arr_new_reach_probs = vector<vector<vector<float>>>(node->getChildrens().size());
        // 构造reach probs矩阵
        for(std::size_t action_ind = 0;action_ind < node->getChildrens().size();action_ind ++){
            if(best_respond_arr_new_reach_probs[action_ind].empty()){
                best_respond_arr_new_reach_probs[action_ind] = vector<vector<float>>(this->player_number);
            }
            vector<vector<float>>& next_reach_probs = best_respond_arr_new_reach_probs[action_ind];
            if(next_reach_probs[player].empty()) {
                next_reach_probs[player] = vector<float>(reach_probs[player].size());
                next_reach_probs[1 - player] = vector<float>(reach_probs[1 - player].size());
            }
            //vector<vector<float>> next_reach_probs(this->player_number);
            for(int i = 0;i < this->player_number;i ++){
                if(i == node->getPlayer()) {
                    int private_combo_numbers = reach_probs[i].size();
                    for (int j = 0; j < private_combo_numbers; j++) {
                        next_reach_probs[i][j] =
                                reach_probs[node->getPlayer()][j] * node_strategy[action_ind * private_combo_numbers + j];
                    }
                }else{
                    next_reach_probs[i].assign(reach_probs[i].begin(),reach_probs[i].end());
                }
            }


            shared_ptr<GameTreeNode> one_child = node->getChildrens()[action_ind];

#ifdef DEBUG
            if (one_child == nullptr)
                throw runtime_error("child node not found");
#endif
            const vector<float>& action_payoffs = this->bestResponse(one_child,player,next_reach_probs,board,deal);

#ifdef DEBUG
            if (action_payoffs.size() != total_payoffs.size())
                throw runtime_error(
                        tfm::format(
                                "length not match between action payoffs and total payoffs %s -- %s",
                                action_payoffs.size(),total_payoffs.size()
                        )
                );
#endif

            for(std::size_t i = 0 ;i < total_payoffs.size();i ++){
                total_payoffs[i] += action_payoffs[i];//  * node_strategy[i] 的动作实际上已经在递归的时候做过了，所以这里不需要乘
            }
        }
        if(this->debug) {
            cout << ("[action]") << endl;
            node->printHistory();
            //System.out.println(Arrays.toString(total_payoffs));
        }
        return total_payoffs;
    }
}

vector<float>
BestResponse::terminalBestReponse(shared_ptr<TerminalNode> node, int player, const vector<vector<float>>& reach_probs,
                                  uint64_t board, int deal) {
    uint64_t board_long = board;
    int oppo = 1 - player;
    const vector<RiverCombs>& player_combs = this->rrm.getRiverCombos(player,this->pcm.getPreflopCards(player),board);  //this.river_combos[player];
    const vector<RiverCombs>& oppo_combs = this->rrm.getRiverCombos(1 - player,this->pcm.getPreflopCards(1 - player),board);  //this.river_combos[player];

    float player_payoff = node->get_payoffs()[player];

    vector<float> payoffs = vector<float>(this->player_hands[player]);


#ifdef DEBUG
    if(this->player_number != 2) throw runtime_error("player NE 2 not supported");
#endif
    // 对手的手牌可能需要和其reach prob一样长
    vector<float> oppo_card_sum(52);

    //用于记录对手总共的手牌绝对prob之和
    float oppo_prob_sum = 0;

    const vector<float>& oppo_reach_prob = reach_probs[1 - player];
    for(std::size_t oppo_hand = 0;oppo_hand < oppo_combs.size(); oppo_hand ++){
        const RiverCombs& one_hc = oppo_combs[oppo_hand];
        uint64_t one_hc_long  = Card::boardInts2long(one_hc.private_cards.get_hands());

        // 如果对手手牌和public card有重叠，那么这组牌不可能存在
        if(Card::boardsHasIntercept(one_hc_long,board_long)){
            continue;
        }

        oppo_prob_sum += oppo_reach_prob[one_hc.reach_prob_index];
        oppo_card_sum[one_hc.private_cards.card1] += oppo_reach_prob[one_hc.reach_prob_index];
        oppo_card_sum[one_hc.private_cards.card2] += oppo_reach_prob[one_hc.reach_prob_index];
    }


    for(std::size_t player_hand = 0;player_hand < player_combs.size();player_hand ++) {
        const RiverCombs& player_hc = player_combs[player_hand];
        uint64_t player_hc_long = Card::boardInts2long(player_hc.private_cards.get_hands());
        if(Card::boardsHasIntercept(player_hc_long,board_long)){
            payoffs[player_hand] = 0;
        }else{
            int oppo_hand = this->pcm.indPlayer2Player(player,oppo,player_hc.reach_prob_index);
            float add_reach_prob;
            if(oppo_hand == -1){
                add_reach_prob = 0;
            }else{
                add_reach_prob = oppo_reach_prob[oppo_hand];
            }
            payoffs[player_hc.reach_prob_index] = (oppo_prob_sum
                                                   - oppo_card_sum[player_hc.private_cards.card1]
                                                   - oppo_card_sum[player_hc.private_cards.card2]
                                                   + add_reach_prob
                                                  ) * player_payoff;
        }
    }

    if(this->debug) {
        cout << ("[terminal]") << endl;
        node->printHistory();
        //System.out.println(Arrays.toString(payoffs));
    }
    return payoffs;
}

vector<float>
BestResponse::showdownBestResponse(shared_ptr<ShowdownNode> node, int player,const vector<vector<float>>& reach_probs,
                                   uint64_t board, int deal) {
#ifdef DEBUG
    if(this->player_number != 2) throw runtime_error("player number is not 2");
#endif

    int oppo = 1 - player;
    const vector<RiverCombs>& player_combs = this->rrm.getRiverCombos(player,this->pcm.getPreflopCards(player),board);  //this.river_combos[player];
    const vector<RiverCombs>& oppo_combs = this->rrm.getRiverCombos(1 - player,this->pcm.getPreflopCards(1 - player),board);  //this.river_combos[player];

    float win_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE,player)[player];
    float lose_payoff = node->get_payoffs(ShowdownNode::ShowDownResult::NOTTIE,1 - player)[player];

    vector<float> payoffs = vector<float>(player_hands[player]);

    // 计算胜利时的payoff
    float winsum = 0;
    vector<float> card_winsum(52);
    for(std::size_t i = 0;i < card_winsum.size();i ++) card_winsum[i] = 0;

    std::size_t j = 0;
    //if(player_combs.length != oppo_combs.length) throw new RuntimeException("");

    for(std::size_t i = 0;i < player_combs.size();i ++){
        const RiverCombs& one_player_comb = player_combs[i];
        while (j < oppo_combs.size() && one_player_comb.rank < oppo_combs[j].rank){
            const RiverCombs& one_oppo_comb = oppo_combs[j];
            winsum += reach_probs[oppo][one_oppo_comb.reach_prob_index];

            card_winsum[one_oppo_comb.private_cards.card1] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            card_winsum[one_oppo_comb.private_cards.card2] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            j ++;
        }
        payoffs[one_player_comb.reach_prob_index] = (winsum
                                                     - card_winsum[one_player_comb.private_cards.card1]
                                                     - card_winsum[one_player_comb.private_cards.card2]
                                                    ) * win_payoff;
    }

    // 计算失败时的payoff
    float losssum = 0;
    vector<float> card_losssum(52);

    j = oppo_combs.size() - 1;
    for(int i = player_combs.size() - 1;i >= 0;i --){
        const RiverCombs& one_player_comb = player_combs[i];
        while (j >= 0 && one_player_comb.rank > oppo_combs[j].rank){
            const RiverCombs& one_oppo_comb = oppo_combs[j];
            losssum += reach_probs[oppo][one_oppo_comb.reach_prob_index];

            card_losssum[one_oppo_comb.private_cards.card1] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            card_losssum[one_oppo_comb.private_cards.card2] += reach_probs[oppo][one_oppo_comb.reach_prob_index];
            j --;
        }
        payoffs[one_player_comb.reach_prob_index] += (losssum
                                                      - card_losssum[one_player_comb.private_cards.card1]
                                                      - card_losssum[one_player_comb.private_cards.card2]
                                                     ) * lose_payoff;
    }
    if(this->debug) {
        cout << ("[showdown]") << endl;
        node->printHistory();
        //System.out.println(Arrays.toString(payoffs));
    }
    return payoffs;
}

