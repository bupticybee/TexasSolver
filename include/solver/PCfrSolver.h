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
#include <queue>
#include <thread>

template<typename T>
class ThreadsafeQueue {
    mutable std::mutex mutex_;
public:
    std::vector<T> queue_;

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
        T tmp = queue_.back();
        queue_.pop_back();
        return tmp;
    }

    void push(const T &item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.insert(queue_.begin(),item);
    }
};

struct TaskParams{
    int player;
    shared_ptr<GameTreeNode> node;
    vector<float> reach_probs;
    int iter;
    uint64_t current_board;
    int deal;
    vector<float> result;
    public:
    TaskParams(int player, shared_ptr<GameTreeNode> node, const vector<float> &reach_probs, int iter,
                       uint64_t current_board,int deal){
        this->player = player;
        this->node = node;
        this->reach_probs = reach_probs;
        this->iter = iter;
        this->current_board = current_board;
        this->deal = deal;
    }
};


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
    GameTreeNode::GameRound root_round;
    GameTreeNode::GameRound split_round;
    enum TaskType {
        Stage1,
        Stage2,
        Stage3
    };
    TaskType distributing_task;
    ThreadsafeQueue<TaskParams> taskqueue;
    std::vector<std::thread> workers;

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
    void each_worker_do();
    void purnTree();

    std::mutex numlock;
    int ind_task;

};


#endif //TEXASSOLVER_PCFRSOLVER_H
