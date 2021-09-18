//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_TCFRSOLVER_H
#define TEXASSOLVER_TCFRSOLVER_H
#include <include/solver/BestResponse.h>
#include <include/ranges/PrivateCards.h>
#include <include/compairer/Compairer.h>
#include <include/Deck.h>
#include <include/ranges/RiverRangeManager.h>
#include <include/ranges/PrivateCardsManager.h>
#include <include/trainable/CfrPlusTrainable.h>
#include <include/trainable/DiscountedCfrTrainable.h>
#include "include/solver/Solver.h"
//#include <boost/thread/executors/basic_thread_pool.hpp>
//#include <boost/thread/future.hpp>
//#include <boost/thread.hpp>
//#include <boost/thread/detail/nullary_function.hpp>
//#include <boost/thread/csbl/functional.hpp>
//#include <boost/thread/detail/config.hpp>

class TCfrSolver:Solver {
public:
    TCfrSolver(shared_ptr<GameTree> tree,
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
    //shared_ptr<boost::basic_thread_pool> pool;

    const vector<PrivateCards>& playerHands(int player);
    vector<vector<float>> getReachProbs();
    static vector<PrivateCards> noDuplicateRange(const vector<PrivateCards>& private_range,uint64_t board_long);
    void setTrainable(shared_ptr<GameTreeNode> root);
    const vector<float>* cfr(int player, shared_ptr<GameTreeNode> node, const vector<vector<float>>& reach_probs, int iter, uint64_t current_board);
    const vector<float>* chanceUtility(int player,shared_ptr<ChanceNode> node,const vector<vector<float>>& reach_probs,int iter,uint64_t current_board);
    const vector<float>* showdownUtility(int player,shared_ptr<ShowdownNode> node,const vector<vector<float>>& reach_probs,int iter,uint64_t current_board);
    const vector<float>* actionUtility(int player,shared_ptr<ActionNode> node,const vector<vector<float>>& reach_probs,int iter,uint64_t current_board);
    const vector<float>* terminalUtility(int player,shared_ptr<TerminalNode> node,const vector<vector<float>>& reach_prob,int iter,uint64_t current_board);
    //template<typename T, typename F, typename Ex>
    //boost::future<T> fork(Ex& ex, F&& func);
};


#endif //TEXASSOLVER_TCFRSOLVER_H
