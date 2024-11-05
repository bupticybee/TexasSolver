#ifndef _SLICE_CFR_H_
#define _SLICE_CFR_H_

#include <omp.h>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "include/nodes/GameTreeNode.h"
#include "include/solver/PCfrSolver.h"
#include <mutex>
#include <atomic>
#include <utility>

using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::dynamic_pointer_cast;
using std::mutex;

#define N_CARD 52
#define N_PLAYER 2
#define P0 0
#define P1 1
#define CHANCE_PLAYER N_PLAYER

#define N_ROUND 4
#define PREFLOP_ROUND 0
#define FLOP_ROUND    1
#define TURN_ROUND    2
#define RIVER_ROUND   3

#define FOLD_TYPE            0
#define SHOWDOWN_TYPE        1
#define N_LEAF_TYPE          2

#define N_TYPE 5

#define two_card_hash(card1, card2) ((1LL<<(card1)) | (1LL<<(card2)))
#define tril_idx(r, c) (((r)*((r)-1)>>1)+(c)) // r>c>=0

#define get_size(n_act, n_hand) (((n_act) * 4 + 1) * (n_hand))
#define cfv_offset(n_hand, act_idx) ((n_hand) * (act_idx))
#define reach_prob_offset(n_act, n_hand, act_idx) (((n_act) * 3 + (act_idx)) * (n_hand))
#define reach_prob_to_cfv(n_act, n_hand) ((n_act) * (n_hand) * 3)

// 数组poss_card的索引[0,51]-->[1,52],8位二进制编码,最多选两个,占用高16位,低16位预留其他用途
#define code_idx0(i) (((i)+1)<<24)
#define decode_idx0(x) (((x)>>24) - 1)
#define code_idx1(i) (((i)+1)<<16)
#define decode_idx1(x) ((((x)>>16)&0xff) - 1)

#define EXP_TASK 0
#define CFV_TASK 1
#define CFR_TASK 2

struct Node {
    int n_act = 0;// 动作数
    int parent_offset = -1;// 本节点对应的父节点数据reach_prob的偏移量
    float *parent_cfv = nullptr;
    // mutex *mtx = nullptr;
    float *data = nullptr;// cfv,regret_sum,strategy_sum,reach_prob,sum
    float *opp_prob = nullptr;
    size_t board = 0LL;
};
struct LeafNode {
    float *reach_prob[N_PLAYER] = {nullptr,nullptr};
    size_t info = 0;
};
struct PreLeafNode {
    PreLeafNode(float *cfv):cfv(cfv) {}
    float *cfv = nullptr;
    vector<int> leaf_node_idx;
};
struct DFSNode {
    DFSNode(int player, int n_act, int parent_act, int info, int parent_dfs_idx, int parent_p0_act, int parent_p0_idx, int parent_p1_act, int parent_p1_idx)
        :player(player), n_act(n_act), parent_act(parent_act), info(info), parent_dfs_idx(parent_dfs_idx)
        , parent_p0_act(parent_p0_act), parent_p0_idx(parent_p0_idx), parent_p1_act(parent_p1_act), parent_p1_idx(parent_p1_idx) {}
    int player = -1;// 活动玩家(叶子节点时为父节点玩家)
    int n_act = 0;// 动作数
    int parent_act = -1;// 本节点对应的父节点动作索引
    int info = 0;
    int parent_dfs_idx = -1;
    int parent_p0_act = -1;
    int parent_p0_idx = -1;
    int parent_p1_act = -1;
    int parent_p1_idx = -1;
};

struct StrengthData {
    StrengthData(int size, const RiverCombs *p):size(size), data(p) {}
    int size = 0;
    const RiverCombs *data = nullptr;
};

class SliceCFR : public Solver {
public:
    SliceCFR(
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
    );
    virtual ~SliceCFR();
    virtual size_t estimate_tree_size();
    void train();
    vector<float> exploitability();
    void stop();
    json dumps(bool with_status, int depth);
    vector<vector<vector<float>>> get_strategy(shared_ptr<ActionNode> node, vector<Card> cards);
    vector<vector<vector<float>>> get_evs(shared_ptr<ActionNode> node, vector<Card> cards);
protected:
    atomic_bool stop_flag {false};
    bool init_succ = false;
    int n_thread = 0;
    int steps = 0, interval = 0, n_card = N_CARD, min_card = 0;
    int init_round = 0;
    int dfs_idx = 0;// 先序遍历
    unordered_map<ActionNode*, vector<int>> node_idx;
    int combination_num[N_ROUND-1] {1,N_CARD,N_CARD*N_CARD};
    size_t init_board = 0;
    int hand_size[N_PLAYER];
    float norm = 1;// 根节点概率归一化系数
    float tol = 0.01;// exploitability容忍度
    float alpha = 1.5, beta = 0, gamma = 2;
    float pos_coef = 0, neg_coef = 0, coef = 0;
    RiverRangeManager rrm;
    vector<int> hand_card;// p0_card1,p0_card2,p1_card1,p1_card2,相对于min_card的偏移量
    int *hand_card_ptr[N_PLAYER] {nullptr,nullptr};
    vector<size_t> hand_hash;
    size_t *hand_hash_ptr[N_PLAYER] {nullptr,nullptr};
    vector<int> poss_card;
    int chance_branch[N_ROUND];
    int chance_den[N_ROUND];
    vector<int> same_hand_idx;
    int *same_hand_ptr[N_PLAYER] {nullptr,nullptr};
    vector<vector<PrivateCards>> ranges;
    vector<DFSNode> dfs_node;
    vector<int> dfs_idx_map;// dfs遍历的每个节点在内存中的索引
    int node_cnt[N_TYPE];
    int n_leaf_node = 0;
    int n_player_node = 0;
    vector<vector<int>> leaf_node_dfs;
    vector<int> chance_node;
    vector<vector<float>> ev;
    float *ev_ptr = nullptr;
    vector<vector<vector<int>>> slice;
    vector<vector<int>> slice_offset;
    vector<float> root_cfv, root_prob;// P0_cfv,P1_cfv,P0_prob,P1_prob
    float *root_prob_ptr[N_PLAYER] {nullptr,nullptr};
    float *root_cfv_ptr[N_PLAYER] {nullptr,nullptr};
    // shared_ptr<GameTree> tree = nullptr;
    Deck& deck;
    void init();
    void init_hand_card(vector<PrivateCards> &range1, vector<PrivateCards> &range2);
    void init_hand_card(vector<PrivateCards> &range, vector<int> &cards, vector<float> &prob, size_t board, vector<PrivateCards> &out);
    void init_same_hand_idx();
    void init_min_card();
    virtual size_t init_memory();
    size_t init_player_node();
    size_t init_leaf_node();
    void set_cfv_and_offset(DFSNode &node, int player, float *&cfv, int &offset);
    void normalization();
    size_t init_strength_table();
    void dfs(shared_ptr<GameTreeNode> node, int parent_act=-1, int parent_dfs_idx=-1, int parent_p0_act=-1, int parent_p0_idx=-1, int parent_p1_act=-1, int parent_p1_idx=-1, int cnt0=0, int cnt1=0, int info=0);
    void init_poss_card(Deck& deck, size_t board);
    virtual void step(int iter, int player, int task);
    virtual void leaf_cfv(int player);
    void fold_cfv(int player, float *cfv, float *opp_reach, int my_hand, float val, size_t board);
    void sd_cfv(int player, float *cfv, float *opp_reach, int my_hand, int opp_hand, float val, int idx);
    void append_node_idx(int p_idx, int act_idx, int player, int cpu_node_idx);
    vector<vector<int>> pre_leaf_node_map;// [dfs_idx,act_idx]
    vector<vector<PreLeafNode>> pre_leaf_node;// [player,idx]
    vector<vector<int>> root_child_idx;
    vector<LeafNode> leaf_node;
    vector<Node> player_node;
    Node *player_node_ptr = nullptr;
    int sd_offset = 0;
    // vector<mutex> mtx;
    // vector<vector<int>> mtx_map;
    // int mtx_idx = N_PLAYER;
    vector<vector<StrengthData>> strength;
    size_t _estimate_tree_size(shared_ptr<GameTreeNode> node);
    virtual void _reach_prob(int player, bool avg_strategy);
    virtual void _rm(int player, bool avg_strategy);
    virtual void clear_data(int player);
    virtual void clear_root_cfv();
    virtual void post_process() {}
    json reConvertJson(const shared_ptr<GameTreeNode>& node, int depth, int max_depth, int &idx, int info);
    virtual vector<vector<float>> get_avg_strategy(int idx);// [n_hand,n_act]
    virtual vector<vector<float>> get_ev(int idx);// [n_hand,n_act]
    bool print_exploitability(int iter, Timer &timer);
    virtual void cfv_to_ev();
    void cfv_to_ev(Node *node, int player);
    void get_prob_sum(vector<float> &prob_sum, float &sum, int player, float *reach_prob, size_t board);
    void output_data(ActionNode *node, vector<Card> &cards, vector<vector<vector<float>>> &out, bool ev);
};

#endif // _SLICE_CFR_H_
