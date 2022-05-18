//
// Created by Xuefeng Huang on 2020/2/1.
//

#ifndef TEXASSOLVER_BESTRESPONSE_H
#define TEXASSOLVER_BESTRESPONSE_H
#include <omp.h>
#include <include/ranges/PrivateCards.h>
#include <include/compairer/Compairer.h>
#include <include/Deck.h>
#include <include/ranges/RiverRangeManager.h>
#include <include/ranges/PrivateCardsManager.h>
#include <include/trainable/CfrPlusTrainable.h>
#include <include/trainable/DiscountedCfrTrainable.h>
#include <include/nodes/ChanceNode.h>
#include <include/nodes/TerminalNode.h>
#include <include/nodes/ShowdownNode.h>
#include <include/tools/utils.h>

using namespace std;

class BestResponse {
private:
    Deck& deck;
    // player -> preflop combos
    vector<vector<PrivateCards>>& private_combos;
    vector<int> player_hands;
    int player_number;
    RiverRangeManager& rrm;
    PrivateCardsManager& pcm;
    bool debug;
    vector<vector<float>> reach_probs;
    int nthreads;
    int use_halffloats;
public:
    BestResponse(
            vector<vector<PrivateCards>>& private_combos,
            int player_number,
            PrivateCardsManager& pcm,
            RiverRangeManager& rrm,
            Deck& deck,
            bool debug,
            int color_iso_offset[][4],
            GameTreeNode::GameRound split_round,
            int nthreads = 1,
            int use_halffloats = 0
            );
    float printExploitability(shared_ptr<GameTreeNode> root, int iterationCount, float initial_pot, uint64_t initialBoard);
    float getBestReponseEv(shared_ptr<GameTreeNode> node, int player,vector<vector<float>> reach_probs, uint64_t initialBoard,int deal);

private:
    vector<float> bestResponse(shared_ptr<GameTreeNode> node, int player, const vector<vector<float>>& reach_probs, uint64_t board,int deal);
    vector<float> chanceBestReponse(shared_ptr<ChanceNode> node, int player, const vector<vector<float>>& reach_probs, uint64_t current_board,int deal);
    vector<float> actionBestResponse(shared_ptr<ActionNode> node, int player, const vector<vector<float>>& reach_probs, uint64_t board,int deal);
    vector<float> terminalBestReponse(shared_ptr<TerminalNode> node, int player, const vector<vector<float>>& reach_probs, uint64_t board,int deal);
    vector<float> showdownBestResponse(shared_ptr<ShowdownNode> node, int player, const vector<vector<float>>& reach_probs,uint64_t board,int deal);
    int color_iso_offset[52 * 52 * 2][4];
    GameTreeNode::GameRound split_round;
};


#endif //TEXASSOLVER_BESTRESPONSE_H
