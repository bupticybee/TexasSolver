//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PCFRSOLVER_H
#define TEXASSOLVER_PCFRSOLVER_H
#include <ranges/PrivateCards.h>
#include <compairer/Compairer.h>
#include <Deck.h>
#include <ranges/RiverRangeManager.h>
#include <ranges/PrivateCardsManager.h>
#include <trainable/CfrPlusTrainable.h>
#include <trainable/DiscountedCfrTrainable.h>
#include "Solver.h"
#include <omp.h>
#include "tools/lookup8.h"

class PCfrSolver:public Solver {
public:
    PCfrSolver(shared_ptr<GameTree> tree,
            vector<PrivateCards> range1 ,
            vector<PrivateCards> range2,
            vector<int> initial_board,
            shared_ptr<Compairer> compairer,
            Deck deck,
            int iteration_number,
            bool debug,
            int print_interval,
            string logfile,
            string trainer,
            Solver::MonteCarolAlg monteCarolAlg,
            int warmup,
            int num_threads
    );
    void train() override;
private:
    vector<vector<PrivateCards>> ranges;
    vector<PrivateCards> range1;
    vector<PrivateCards> range2;
    vector<int> initial_board;
    uint64_t initial_board_long;
    shared_ptr<Compairer> compairer;

    Deck deck;
    RiverRangeManager rrm;
    int player_number;
    int iteration_number;
    PrivateCardsManager pcm;
    bool debug;
    int print_interval;
    string trainer;
    string logfile;
    Solver::MonteCarolAlg monteCarolAlg;
    vector<int> round_deal;
    int num_threads;
    int warmup;

    const vector<PrivateCards>& playerHands(int player);
    vector<vector<float>> getReachProbs();
    static vector<PrivateCards> noDuplicateRange(const vector<PrivateCards>& private_range,uint64_t board_long);
    void setTrainable(shared_ptr<GameTreeNode> root);
    vector<float> cfr(int player, shared_ptr<GameTreeNode> node, const vector<float>& reach_probs, int iter, vector<uint64_t> current_boards,vector<int> deals);
    vector<int> getAllAbstractionDeal(int deal);
    vector<float> chanceUtility(int player,shared_ptr<ChanceNode> node,const vector<float>& reach_probs,int iter,vector<uint64_t> current_boards,vector<int> deals);
    vector<float> showdownUtility(int player,shared_ptr<ShowdownNode> node,const vector<float>& reach_probs,int iter,vector<uint64_t> current_boards,vector<int> deals);
    vector<float> actionUtility(int player,shared_ptr<ActionNode> node,const vector<float>& reach_probs,int iter,vector<uint64_t> current_boards,vector<int> deals);
    vector<float> terminalUtility(int player,shared_ptr<TerminalNode> node,const vector<float>& reach_prob,int iter,vector<uint64_t> current_boards,vector<int> deals);
    void findGameSpecificIsomorphisms();
    void purnTree();


};


#endif //TEXASSOLVER_PCFRSOLVER_H
