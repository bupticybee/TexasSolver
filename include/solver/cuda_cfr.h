#ifndef _CUDA_CFR_H_
#define _CUDA_CFR_H_

#include <omp.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "include/nodes/GameTreeNode.h"
#include "include/solver/PCfrSolver.h"
#include <mutex>
#include <utility>
#include "cuda_runtime.h"
#include "include/solver/slice_cfr.h"

#define LANE_SIZE 32

struct CudaLeafNode {
    float val = 0;// fold:player0的收益*随机概率,sd:胜者收益*随机概率
    int offset_prob_sum = 0;
    int offset_p0 = 0;
    int offset_p1 = 0;
    float *data_p0 = nullptr;
    float *data_p1 = nullptr;
    int *info = nullptr;
};
struct SDNode {
    float val = 0;// 胜者收益*随机概率
    int offset_prob_sum = 0;
    int offset_p0 = 0;
    int offset_p1 = 0;
    float *data_p0 = nullptr;
    float *data_p1 = nullptr;
    int *strength_data = nullptr;
};

class CudaCFR : public SliceCFR {
public:
    CudaCFR(
        shared_ptr<GameTree> tree,
        vector<PrivateCards> &range1,
        vector<PrivateCards> &range2,
        vector<int> &initial_board,
        shared_ptr<Compairer> compairer,
        Deck &deck,
        int train_step,
        int print_interval,
        float accuracy,
        int n_thread,
        Logger *logger
    ):SliceCFR(tree, range1, range2, initial_board, compairer, deck, train_step, print_interval, accuracy, n_thread, logger) {}
    virtual ~CudaCFR();
    virtual size_t estimate_tree_size();
protected:
    int *dev_hand_card = nullptr;
    int *dev_hand_card_ptr[N_PLAYER] {nullptr,nullptr};
    size_t *dev_hand_hash = nullptr;
    size_t *dev_hand_hash_ptr[N_PLAYER] {nullptr,nullptr};
    int *dev_same_hand_idx = nullptr;
    Node *dev_nodes = nullptr;// cuda内存地址
    CudaLeafNode *dev_leaf_node = nullptr;// cuda内存地址
    vector<float*> dev_data;
    vector<int*> dev_strength;
    float *dev_root_cfv = nullptr, *dev_prob_sum = nullptr;
    virtual size_t init_memory();
    size_t init_player_node();
    size_t init_leaf_node();
    void set_cfv_and_offset(DFSNode &node, int player, float *&cfv, int &offset);
    size_t init_strength_table();
    virtual void step(int iter, int player, int task);
    virtual void leaf_cfv(int player);
    int block_size(int size) {// ceil
        return (size + LANE_SIZE - 1) / LANE_SIZE;
    }
    void clear_prob_sum(int len);
    virtual void _reach_prob(int player, bool avg_strategy);
    virtual void _rm(int player, bool avg_strategy);
    virtual void clear_data(int player);
    virtual void clear_root_cfv();
    virtual void post_process();
    virtual vector<vector<float>> get_avg_strategy(int idx);
    virtual vector<vector<float>> get_ev(int idx);
    virtual void cfv_to_ev();
};

#endif // _CUDA_CFR_H_
