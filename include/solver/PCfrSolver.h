//
// Created by Xuefeng Huang on 2020/1/31.
//

#ifndef TEXASSOLVER_PCFRSOLVER_H
#define TEXASSOLVER_PCFRSOLVER_H
#include <include/ranges/PrivateCards.h>
#include <include/compairer/Compairer.h>
#include <include/Deck.h>
#include <include/ranges/RiverRangeManager.h>
#include <include/ranges/PrivateCardsManager.h>
#include <include/trainable/CfrPlusTrainable.h>
#include <include/trainable/DiscountedCfrTrainable.h>
#include "include/solver/Solver.h"
#include <omp.h>
#include "include/tools/lookup8.h"
#include "include/tools/utils.h"
#include <queue>
#include <optional>
/*
template<typename T>
class ThreadsafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;

    // Moved out of public interface to prevent races between this
    // and pop().
    bool empty() const {
        return queue_.empty();
    }

public:
    ThreadsafeQueue() = default;
    ThreadsafeQueue(const ThreadsafeQueue<T> &) = delete ;
    ThreadsafeQueue& operator=(const ThreadsafeQueue<T> &) = delete ;

    ThreadsafeQueue(ThreadsafeQueue<T>&& other) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_ = std::move(other.queue_);
    }

    virtual ~ThreadsafeQueue() { }

    unsigned long size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    std::optional<T> pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return {};
        }
        T tmp = queue_.front();
        queue_.pop();
        return tmp;
    }

    void push(const T &item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
    }
};

struct TaskParams{
    int player;
    shared_ptr<GameTreeNode> node;
    const vector<float> &reach_probs;
    int iter;
    uint64_t current_board;
    int deal;
};
*/

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
            float accuracy,
            bool use_isomorphism,
            int use_halffloats,
            int num_threads
    );
    ~PCfrSolver();
    void train() override;
    void stop() override;
    json dumps(bool with_status,int depth) override;
    vector<vector<vector<float>>> get_strategy(shared_ptr<ActionNode> node,vector<Card> chance_cards) override;
    vector<vector<vector<float>>> get_evs(shared_ptr<ActionNode> node,vector<Card> chance_cards) override;
private:
    vector<vector<PrivateCards>> ranges;
    vector<PrivateCards> range1;
    vector<PrivateCards> range2;
    vector<int> initial_board;
    uint64_t initial_board_long;
    shared_ptr<Compairer> compairer;
    int color_iso_offset[52 * 52 * 2][4] = {0};
    bool collecting_statics = false;
    bool statics_collected = false;

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
    GameTreeNode::GameRound root_round;
    GameTreeNode::GameRound split_round;
    bool distributing_task;
    float accuracy;
    bool use_isomorphism;
    int use_halffloats;
    bool nowstop = false;

    const vector<PrivateCards>& playerHands(int player);
    vector<vector<float>> getReachProbs();
    static vector<PrivateCards> noDuplicateRange(const vector<PrivateCards>& private_range,uint64_t board_long);
    void setTrainable(shared_ptr<GameTreeNode> root);
    vector<float> cfr(int player, shared_ptr<GameTreeNode> node, const vector<float>& reach_probs, int iter, uint64_t current_board,int deal);
    vector<int> getAllAbstractionDeal(int deal);
    vector<float> chanceUtility(int player,shared_ptr<ChanceNode> node,const vector<float>& reach_probs,int iter,uint64_t current_boardi,int deal);
    vector<float> showdownUtility(int player,shared_ptr<ShowdownNode> node,const vector<float>& reach_probs,int iter,uint64_t current_board,int deal);
    vector<float> actionUtility(int player,shared_ptr<ActionNode> node,const vector<float>& reach_probs,int iter,uint64_t current_board,int deal);
    vector<float> terminalUtility(int player,shared_ptr<TerminalNode> node,const vector<float>& reach_prob,int iter,uint64_t current_board,int deal);
    void findGameSpecificIsomorphisms();
    void purnTree();
    void exchangeRange(json& strategy,int rank1,int rank2,shared_ptr<ActionNode> one_node);
    void reConvertJson(const shared_ptr<GameTreeNode>& node,json& strategy,string key,int depth,int max_depth,vector<string> prefix,int deal,vector<vector<int>> exchange_color_list);

};


#endif //TEXASSOLVER_PCFRSOLVER_H
