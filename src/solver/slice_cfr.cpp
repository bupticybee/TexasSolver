#include <climits>
#include "include/solver/slice_cfr.h"
#include "include/ranges/RiverRangeManager.h"

using std::memory_order_relaxed;
using std::atomic_ref;

void print_array(int *arr, int n) {
    if(arr != nullptr && n > 0) {
        printf("%d", arr[0]);
        for(int i = 1; i < n; i++) printf(",%d", arr[i]);
    }
    printf("\n");
}

void test_parallel_for(int n_thread, int n = 100) {
    vector<int> cnt(n_thread);
    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        cnt[omp_get_thread_num()]++;
    }
    print_array(cnt.data(), n_thread);
}

inline bool cards_valid(size_t hash1, size_t hash2) {
    return (hash1 & hash2) == 0;
}

typedef void (*node_func)(Node *, int);

void rm_avg(Node *node, int n_hand) {
    int size = node->n_act * n_hand;
    int i = 0, h = 0, sum_offset = size << 1;
    float *data = node->data + (size << 1);// strategy_sum
    float sum = 0;
    for(h = 0; h < n_hand; h++) {
        sum = 0;
        for(i = h; i < size; i += n_hand) sum += data[i];
        data[sum_offset+h] = sum;
    }
}
void rm(Node *node, int n_hand) {
    int size = node->n_act * n_hand;
    int i = 0, h = 0, sum_offset = size * 3;
    float *data = node->data + size;// regret_sum
    float sum = 0;
    for(h = 0; h < n_hand; h++) {
        sum = 0;
        for(i = h; i < size; i += n_hand) sum += max(0.0f, data[i]);
        data[sum_offset+h] = sum;
    }
}
void reach_prob_avg(Node *node, int n_hand) {
    int n_act = node->n_act, size = n_act * n_hand;
    int i = 0, h = 0, sum_offset = size << 1;
    float *data = node->data + (size << 1);// strategy_sum
    float *parent_prob = node->parent_cfv + node->parent_offset, temp = 0;
    for(h = 0; h < n_hand; h++) {
        if(data[sum_offset+h] == 0) {// 1/n_act
            temp = parent_prob[h] / n_act;
            for(i = h; i < size; i += n_hand) data[size+i] = temp;
        }
        else {
            temp = parent_prob[h] / data[sum_offset+h];
            for(i = h; i < size; i += n_hand) data[size+i] = temp * data[i];
        }
    }
}
void reach_prob(Node *node, int n_hand) {
    int n_act = node->n_act, size = n_act * n_hand;
    int i = 0, h = 0, rp_offset = size << 1, sum_offset = rp_offset + size;
    float *data = node->data + size;// regret_sum
    float *parent_prob = node->parent_cfv + node->parent_offset, temp = 0;
    for(h = 0; h < n_hand; h++) {
        if(data[sum_offset+h] == 0) {// 1/n_act
            temp = parent_prob[h] / n_act;
            for(i = h; i < size; i += n_hand) data[rp_offset+i] = temp;
        }
        else {
            temp = parent_prob[h] / data[sum_offset+h];
            for(i = h; i < size; i += n_hand) data[rp_offset+i] = temp * max(0.0f, data[i]);
        }
    }
}
// 子节点cfv取最大值
void best_cfv_up(Node *node, int n_hand) {
    int size = node->n_act * n_hand;
    int i = 0, h = 0;
    float *parent_cfv = node->parent_cfv, *cfv = node->data, val = 0;
    // mutex *mtx = node->mtx;
    for(h = 0; h < n_hand; h++) {
        val = cfv[h];// 第一个
        for(i = h+n_hand; i < size; i += n_hand) val = max(val, cfv[i]);
        // mtx->lock();
        // parent_cfv[h] += val;// 需要加锁
        // mtx->unlock();
        atomic_ref(parent_cfv[h]).fetch_add(val, memory_order_relaxed);
    }
    for(i = 0; i < size; i++) cfv[i] = 0;// 清零cfv
}
// 子节点cfv加权求和
void cfv_up(Node *node, int n_hand) {
    int n_act = node->n_act, size = n_act * n_hand;
    int i = 0, h = 0, sum_offset = size << 2;
    float *parent_cfv = node->parent_cfv, *cfv = node->data, val = 0;
    float *regret_sum = cfv + size;
    // mutex *mtx = node->mtx;
    for(h = 0; h < n_hand; h++) {
        val = 0;
        if(cfv[sum_offset+h] == 0) {
            for(i = h; i < size; i += n_hand) val += cfv[i];
            val /= n_act;// uniform strategy
        }
        else {
            for(i = h; i < size; i += n_hand) {
                val += cfv[i] * max(0.0f, regret_sum[i]);
            }
            val /= cfv[sum_offset+h];
        }
        // cfv[sum_offset+h] = val;
        // mtx->lock();
        // parent_cfv[h] += val;// 需要加锁
        // mtx->unlock();
        atomic_ref(parent_cfv[h]).fetch_add(val, memory_order_relaxed);
        for(i = h; i < size; i += n_hand) regret_sum[i] += cfv[i] - val;// 更新regret_sum
        val = 0;
        for(i = h; i < size; i += n_hand) val += max(0.0f, regret_sum[i]);
        cfv[sum_offset+h] = val;// 求和
    }
    for(i = 0; i < size; i++) cfv[i] = 0;// 清零cfv
}
// 只计算cfv
void cfv_up_avg(Node *node, int n_hand) {
    int n_act = node->n_act, size = n_act * n_hand;
    int i = 0, h = 0, sum_offset = size << 2;
    float *parent_cfv = node->parent_cfv, *cfv = node->data, val = 0;
    float *strategy_sum = cfv + (size << 1);
    // mutex *mtx = node->mtx;
    for(h = 0; h < n_hand; h++) {
        val = 0;
        if(cfv[sum_offset+h] == 0) {
            for(i = h; i < size; i += n_hand) val += cfv[i];
            val /= n_act;// uniform strategy
        }
        else {
            for(i = h; i < size; i += n_hand) {
                val += cfv[i] * strategy_sum[i];
            }
            val /= cfv[sum_offset+h];
        }
        // cfv[sum_offset+h] = val;
        // mtx->lock();
        // parent_cfv[h] += val;// 需要加锁
        // mtx->unlock();
        atomic_ref(parent_cfv[h]).fetch_add(val, memory_order_relaxed);
        // for(i = h; i < size; i += n_hand) regret_sum[i] += cfv[i] - val;// 更新regret_sum
        // val = 0;
        // for(i = h; i < size; i += n_hand) val += max(0.0f, regret_sum[i]);
        // cfv[sum_offset+h] = val;// 求和
    }
    // for(i = 0; i < size; i++) cfv[i] = 0;// 清零cfv
}
// 在cfv_up前执行
void discount_data(Node *node, int n_hand, float pos_coef, float neg_coef, float coef) {
    int size = node->n_act * n_hand, i = 0;
    float *regret_sum = node->data + size, *strategy_sum = regret_sum + size;
    for(i = 0; i < size; i++) {
        regret_sum[i] *= regret_sum[i] > 0 ? pos_coef : neg_coef;
        strategy_sum[i] = strategy_sum[i] * coef + strategy_sum[size+i];
    }
}

void SliceCFR::cfv_to_ev() {
    for(int i = 0; i < N_PLAYER; i++) {
        vector<int>& offset = slice_offset[i];
        #pragma omp parallel for
        for(int j = offset[0]; j < offset.back(); j++) {
            cfv_to_ev(player_node_ptr+j, i);
        }
    }
}
void SliceCFR::cfv_to_ev(Node *node, int player) {
    float *opp_reach = node->opp_prob, *cfv = node->data;
    size_t board = node->board;
    vector<float> opp_prob_sum(n_card, 0);
    float prob_sum = 0, temp = 0;
    get_prob_sum(opp_prob_sum, prob_sum, 1-player, opp_reach, board);
    int n_hand = hand_size[player], size = node->n_act * n_hand, h = 0, i = 0;
    int *same_hand = same_hand_ptr[player], *my_card = hand_card_ptr[player];
    size_t *my_hash = hand_hash_ptr[player];
    for(h = 0; h < n_hand; h++) {
        temp = same_hand[h] != -1 ? opp_reach[same_hand[h]] : 0;// 重复计算的部分
        temp = prob_sum - opp_prob_sum[my_card[h]] - opp_prob_sum[my_card[h+n_hand]] + temp;
        if((my_hash[h] & board) || temp == 0) {
            for(i = h; i < size; i += n_hand) cfv[i] = 0;
        }
        else {
            for(i = h; i < size; i += n_hand) cfv[i] /= temp;
        }
    }
}

// #define TIME_LOG
#ifdef TIME_LOG
atomic_ullong fold_time[16] = {0}, sd_time[16] = {0};
#endif

void SliceCFR::leaf_cfv(int player) {
#ifdef TIME_LOG
    Timer timer;
    for(int i = 0; i < n_thread; i++) {
        fold_time[i].store(0), sd_time[i].store(0);
    }
#endif
    int opp = 1 - player;
    int my_hand = hand_size[player], opp_hand = hand_size[opp];
    vector<PreLeafNode> &vec = pre_leaf_node[player];
    int64_t n = vec.size();
    #pragma omp parallel for schedule(dynamic)
    // #pragma omp parallel for
    for(int64_t i = 0; i < n; i++) {
        // printf("omp_get_thread_num():%d,%zd\n", omp_get_thread_num(), i);
        float *cfv = vec[i].cfv;
        // for(int j = 0; j < my_hand; j++) cfv[j] = 0;
        for(int j : vec[i].leaf_node_idx) {
            LeafNode &node = leaf_node[j];
            if(j < sd_offset) {
                fold_cfv(player, cfv, node.reach_prob[opp], my_hand, ev_ptr[j], node.info);
            }
            else sd_cfv(player, cfv, node.reach_prob[opp], my_hand, opp_hand, ev_ptr[j], node.info);
        }
    }
#ifdef TIME_LOG
    for(int i = 0; i < n_thread; i++) {
        printf("%zd\t%zd\n", fold_time[i].load(), sd_time[i].load());
    }
    // printf("leaf_cfv:%zd ms\n", timer.ms());
#endif
}
void SliceCFR::get_prob_sum(vector<float> &prob_sum, float &sum, int player, float *reach_prob, size_t board) {
    float temp = 0;
    int n_hand = hand_size[player], *hand_card = hand_card_ptr[player];
    size_t *hand_hash = hand_hash_ptr[player];
    for(int i = 0; i < n_hand; i++) {
        if(hand_hash[i] & board) continue;// 对方手牌与公共牌冲突
        temp = reach_prob[i];
        prob_sum[hand_card[i]] += temp;// card1
        prob_sum[hand_card[i+n_hand]] += temp;// card2
        sum += temp;
    }
}
void SliceCFR::fold_cfv(int player, float *cfv, float *opp_reach, int my_hand, float val, size_t board) {
#ifdef TIME_LOG
    Timer timer;
#endif
    if(player != P0) val = -val;
    size_t *my_hash = hand_hash_ptr[player];
    int *my_card = hand_card_ptr[player];
    int *same_hand = same_hand_ptr[player], i = 0;
    vector<float> opp_prob_sum(n_card, 0);
    float prob_sum = 0, temp = 0;
    get_prob_sum(opp_prob_sum, prob_sum, 1-player, opp_reach, board);
    for(i = 0; i < my_hand; i++) {
        if(my_hash[i] & board) {
            // cfv[i] = 0;// 与公共牌冲突，cfv为0
            continue;
        }
        temp = same_hand[i] != -1 ? opp_reach[same_hand[i]] : 0;// 重复计算的部分
        cfv[i] += (prob_sum - opp_prob_sum[my_card[i]] - opp_prob_sum[my_card[i+my_hand]] + temp) * val;
    }
#ifdef TIME_LOG
    fold_time[omp_get_thread_num()] += timer.us();
#endif
}
void SliceCFR::sd_cfv(int player, float *cfv, float *opp_reach, int my_hand, int opp_hand, float val, int idx) {
#ifdef TIME_LOG
    Timer timer;
#endif
    vector<StrengthData> &vec = strength[idx];
    const RiverCombs *my_data = vec[player].data, *opp_data = vec[1-player].data;
    int my_size = vec[player].size, opp_size = vec[1-player].size, i = 0, j = 0, h = 0, rank = 0;
    int *my_card = hand_card_ptr[player], *opp_card = hand_card_ptr[1-player];
    vector<float> opp_prob_sum(n_card, 0);
    float prob_sum = 0;
    for(i = 0, j = 0; i < my_size; i++) {// strength值变小,己方手牌变强
        rank = my_data[i].rank;
        for(; j < opp_size && opp_data[j].rank > rank; j++) {// (胜过对方条件下)找到对方的最强手牌
            h = opp_data[j].reach_prob_index;
            opp_prob_sum[opp_card[h]] += opp_reach[h];// card1
            opp_prob_sum[opp_card[h+opp_hand]] += opp_reach[h];// card2
            prob_sum += opp_reach[h];
        }
        h = my_data[i].reach_prob_index;
        cfv[h] += (prob_sum - opp_prob_sum[my_card[h]] - opp_prob_sum[my_card[h+my_hand]]) * val;
    }
    prob_sum = 0;
    for(h = 0; h < n_card; h++) opp_prob_sum[h] = 0;
    for(i = my_size-1, j = opp_size-1; i >= 0; i--) {// strength值变大,己方手牌变弱
        rank = my_data[i].rank;
        for(; j >= 0 && opp_data[j].rank < rank; j--) {// (败给对方条件下)找到对方的最弱手牌
            h = opp_data[j].reach_prob_index;
            opp_prob_sum[opp_card[h]] += opp_reach[h];// card1
            opp_prob_sum[opp_card[h+opp_hand]] += opp_reach[h];// card2
            prob_sum += opp_reach[h];
        }
        h = my_data[i].reach_prob_index;
        cfv[h] += (opp_prob_sum[my_card[h]] + opp_prob_sum[my_card[h+my_hand]] - prob_sum) * val;
    }
#ifdef TIME_LOG
    sd_time[omp_get_thread_num()] += timer.us();
#endif
}
void SliceCFR::append_node_idx(int p_idx, int act_idx, int player, int leaf_node_idx) {
    if(p_idx == -1) {
        root_child_idx[player].push_back(leaf_node_idx);
        leaf_node[leaf_node_idx].reach_prob[player] = root_prob_ptr[player];
        return;
    }
    vector<PreLeafNode> &vec = pre_leaf_node[player];
    int n_hand = hand_size[player], offset = reach_prob_to_cfv(dfs_node[p_idx].n_act, n_hand);
    float *cfv = player_node[dfs_idx_map[p_idx]].data + cfv_offset(n_hand, act_idx);
    if(pre_leaf_node_map[p_idx].empty()) pre_leaf_node_map[p_idx] = vector<int>(dfs_node[p_idx].n_act, -1);
    int &i = pre_leaf_node_map[p_idx][act_idx];
    if(i == -1) {// 未初始化
        i = vec.size();
        vec.emplace_back(cfv);
    }
    vec[i].leaf_node_idx.push_back(leaf_node_idx);
    leaf_node[leaf_node_idx].reach_prob[player] = cfv + offset;
}
size_t SliceCFR::init_leaf_node() {
    pre_leaf_node_map = vector<vector<int>>(dfs_idx);
    pre_leaf_node = vector<vector<PreLeafNode>>(N_PLAYER);
    root_child_idx = vector<vector<int>>(N_PLAYER);
    leaf_node = vector<LeafNode>(n_leaf_node);
    int node_idx = 0;
    for(int i = 0; i < N_LEAF_TYPE; i++) {
        for(int idx : leaf_node_dfs[i]) {
            DFSNode &node = dfs_node[idx];
            append_node_idx(node.parent_p0_idx, node.parent_p0_act, P0, node_idx);
            append_node_idx(node.parent_p1_idx, node.parent_p1_act, P1, node_idx);
            int j = decode_idx0(node.info), k = decode_idx1(node.info);
            size_t info = init_board;
            if(i == FOLD_TYPE) {
                if(j != -1) info |= 1LL << poss_card[j];
                if(k != -1) info |= 1LL << poss_card[k];
            }
            else {
                if(j == -1) info = 0;
                else if(k == -1) info = j;
                else info = tril_idx(max(j, k), min(j, k));
            }
            leaf_node[node_idx++].info = info;
        }
    }
    sd_offset = leaf_node_dfs[FOLD_TYPE].size();
    logger->log("%zd,%zd", pre_leaf_node[P0].size(), pre_leaf_node[P1].size());
    logger->log("%d,%d,%zd,%zd", n_leaf_node, node_idx, root_child_idx[P0].size(), root_child_idx[P1].size());
    
    size_t max_val[N_PLAYER] = {0, 0}, min_val[N_PLAYER] = {INT_MAX, INT_MAX};
    for(int i = 0; i < N_PLAYER; i++) {
        if(!root_child_idx[i].empty()) {
            pre_leaf_node[i].emplace_back(root_cfv_ptr[i]);
            pre_leaf_node[i].back().leaf_node_idx = root_child_idx[i];
        }
        for(PreLeafNode &node : pre_leaf_node[i]) {
            assert(node.cfv != nullptr);
            max_val[i] = max(max_val[i], node.leaf_node_idx.size());
            min_val[i] = min(min_val[i], node.leaf_node_idx.size());
        }
    }
    logger->log("%zd,%zd,%zd,%zd", min_val[P0], max_val[P0], min_val[P1], max_val[P1]);
    
    ev[FOLD_TYPE].insert(ev[FOLD_TYPE].end(), ev[SHOWDOWN_TYPE].begin(), ev[SHOWDOWN_TYPE].end());
    ev[SHOWDOWN_TYPE].clear();
    ev_ptr = ev[FOLD_TYPE].data();
    size_t total = n_leaf_node * sizeof(LeafNode);
    total += (pre_leaf_node[P0].size() + pre_leaf_node[P1].size()) * sizeof(PreLeafNode);
    total += n_leaf_node * N_PLAYER * sizeof(int);// leaf_node_idx
    return total;
}

SliceCFR::SliceCFR(
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
):deck(deck), steps(train_step), interval(print_interval), n_thread(max(0,n_thread)), rrm(compairer), Solver(tree, logger) {
    init_board = Card::boardInts2long(initial_board);
    init_round = GameTreeNode::gameRound2int(tree->getRoot()->getRound());
    if(init_round < FLOP_ROUND) return;
    init_hand_card(range1, range2);
    if(hand_size[P0] == 0 || hand_size[P1] == 0) return;
    init_same_hand_idx();
    init_min_card();
    init_poss_card(deck, init_board);
    normalization();
    tol = accuracy / 100 * tree->getRoot()->getPot();
    if(this->n_thread == 0) this->n_thread = omp_get_num_procs();
    omp_set_num_threads(this->n_thread);
    // test_parallel_for(this->n_thread);
}

void SliceCFR::init() {
    float unit = 1 << 20;
    size_t size = estimate_tree_size();
    logger->log("estimate memory:%f MB", size/unit);
    
    leaf_node_dfs.resize(N_LEAF_TYPE);
    ev.resize(N_LEAF_TYPE);
    slice.resize(N_PLAYER);
    dfs_idx = 0;
    dfs(tree->getRoot(), -1, -1, -1, -1, -1, -1, 0, 0, 0);
    
    print_array(node_cnt, N_TYPE);
    for(int i = 0; i < N_LEAF_TYPE; i++) {
        printf("%zd,", leaf_node_dfs[i].size());
        assert(node_cnt[i] == leaf_node_dfs[i].size());
    }
    for(int player = P0; player < N_PLAYER; player++) {
        size = 0;
        for(vector<int> &nodes : slice[player]) size += nodes.size();
        printf("%zd,", size);
        assert(size == node_cnt[N_LEAF_TYPE+player]);
    }
    printf("%zd\n", chance_node.size());
    assert(node_cnt[N_LEAF_TYPE+CHANCE_PLAYER] == chance_node.size());

    if(dfs_idx == 0 || dfs_node[0].n_act == 0) return;
    size = init_memory();
    logger->log("%d nodes, total:%f MB", dfs_idx, size/unit);
    init_succ = true;
}

SliceCFR::~SliceCFR() {
    for(Node &node : player_node) {
        if(node.data) free(node.data);
    }
}

void SliceCFR::set_cfv_and_offset(DFSNode &node, int player, float *&cfv, int &offset) {
    if(player == -1) player = node.player;// 向上连接同玩家节点
    int p_idx = node.parent_p0_idx, act_idx = node.parent_p0_act;// 向上连接P0
    if(player != P0) {// 向上连接P1
        p_idx = node.parent_p1_idx;
        act_idx = node.parent_p1_act;
    }
    if(p_idx == -1) {
        cfv = root_cfv_ptr[player];
        offset = root_prob_ptr[player] - root_cfv_ptr[player];
        // mtx = (mutex *)player;
    }
    else {
        if(player != dfs_node[p_idx].player) throw runtime_error("player mismatch");
        cfv = player_node[dfs_idx_map[p_idx]].data + cfv_offset(hand_size[player], act_idx);
        offset = reach_prob_to_cfv(dfs_node[p_idx].n_act, hand_size[player]);
        // if(mtx_map[p_idx].empty()) mtx_map[p_idx] = vector<int>(dfs_node[p_idx].n_act, -1);
        // int &i = mtx_map[p_idx][act_idx];
        // if(i == -1) i = mtx_idx++;
        // mtx = (mutex *)i;
    }
}

size_t SliceCFR::init_player_node() {
    size_t total = 0, size = 0;
    player_node = vector<Node>(n_player_node);
    player_node_ptr = player_node.data();
    dfs_idx_map = vector<int>(dfs_idx, -1);
    slice_offset = vector<vector<int>>(N_PLAYER);
    // mtx_map = vector<vector<int>>(dfs_idx);
    // mtx_idx = N_PLAYER;
    int mem_idx = 0;
    for(int i = 0; i < N_PLAYER; i++) {// 枚举player
        for(vector<int> &nodes : slice[i]) {// 枚举slice
            slice_offset[i].push_back(mem_idx);
            for(int idx : nodes) {// 枚举node
                dfs_idx_map[idx] = mem_idx++;
            }
        }
        slice_offset[i].push_back(mem_idx);
    }
    for(int idx = 0; idx < dfs_idx; idx++) {
        if(dfs_idx_map[idx] == -1) continue;
        DFSNode &node = dfs_node[idx];
        if(node.player != P0 && node.player != P1) throw runtime_error("unknow player");
        Node &target = player_node[dfs_idx_map[idx]];
        target.n_act = node.n_act;
        set_cfv_and_offset(node, -1, target.parent_cfv, target.parent_offset);
        float *ptr = nullptr;
        int offset = 0;
        set_cfv_and_offset(node, 1-node.player, ptr, offset);
        target.opp_prob = ptr + offset;
        target.board = init_board;
        int j = decode_idx0(node.info), k = decode_idx1(node.info);
        if(j != -1) target.board |= 1LL << poss_card[j];
        if(k != -1) target.board |= 1LL << poss_card[k];
        size = get_size(node.n_act, hand_size[node.player]) * sizeof(float);
        target.data = (float *)malloc(size);
        if(target.data == nullptr) throw runtime_error("malloc error");
        total += size;
    }
    // mtx = vector<mutex>(mtx_idx);
    // printf("%d,%d,%d\n", sizeof(mutex), mtx_idx, mtx_idx * sizeof(mutex));
    // total += mtx_idx * sizeof(mutex);
    // for(int i : dfs_idx_map) {
    //     if(i == -1) continue;
    //     player_node[i].mtx = &mtx[(size_t)(player_node[i].mtx)];
    // }
    total += n_player_node * sizeof(Node);
    return total;
}

size_t SliceCFR::init_memory() {
    size_t total = 0;
    int n = root_prob.size();
    root_cfv = vector<float>(n<<1, 0);
    for(int i = 0; i < n; i++) root_cfv[n+i] = root_prob[i];
    total += n * 3 * sizeof(float);
    root_cfv_ptr[P0] = root_cfv.data();
    root_cfv_ptr[P1] = root_cfv_ptr[P0] + hand_size[P0];
    root_prob_ptr[P0] = root_cfv_ptr[P0] + n;
    root_prob_ptr[P1] = root_prob_ptr[P0] + hand_size[P0];
    
    total += init_player_node();
    total += init_leaf_node();
    total += init_strength_table();
    return total;
}

size_t SliceCFR::init_strength_table() {
    int n = poss_card.size();
    vector<size_t> board_hash;
    if(init_round == RIVER_ROUND) board_hash.push_back(init_board);
    else if(init_round == TURN_ROUND) {
        board_hash = vector<size_t>(n, 0);
        for(int i = 0; i < n; i++) board_hash[i] = init_board | (1LL<<poss_card[i]);
    }
    else {// FLOP
        board_hash = vector<size_t>(n*(n-1)>>1, 0);
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                board_hash[tril_idx(j, i)] = init_board | two_card_hash(poss_card[i], poss_card[j]);
            }
        }
    }
    n = board_hash.size();
    strength = vector<vector<StrengthData>>(n);
    // omp_set_num_threads(omp_get_num_procs());
    #pragma omp parallel for
    for(int i = 0; i < n; i++) {
        // printf("omp_get_thread_num():%d,%d\n", omp_get_thread_num(), i);
        const vector<RiverCombs>& p0_comb = rrm.getRiverCombos(P0, ranges[P0], board_hash[i]);
        const vector<RiverCombs>& p1_comb = rrm.getRiverCombos(P1, ranges[P1], board_hash[i]);
        strength[i].emplace_back(p0_comb.size(), p0_comb.data());
        strength[i].emplace_back(p1_comb.size(), p1_comb.data());
    }
    size_t total = (n<<1) * sizeof(StrengthData), size = 0;
    for(int i = 0; i < n; i++) size += strength[i][P0].size + strength[i][P1].size;
    total += (size<<1) * sizeof(int);// rank,idx
    return total;
}

void SliceCFR::init_min_card() {
    min_card = N_CARD;
    int max_card = -1;
    for(int card : hand_card) {
        min_card = min(min_card, card);
        max_card = max(max_card, card);
    }
    n_card = max_card - min_card + 1;// 52张牌中如果只用了连续的一段,可以节省内存
    for(int &card : hand_card) card -= min_card;
}

void SliceCFR::init_hand_card(vector<PrivateCards> &range1, vector<PrivateCards> &range2) {
    ranges = vector<vector<PrivateCards>>(2);
    vector<int> cards;// card1,card2,card1,card2,...
    init_hand_card(range1, cards, root_prob, init_board, ranges[P0]);
    hand_size[P0] = root_prob.size();
    init_hand_card(range2, cards, root_prob, init_board, ranges[P1]);
    hand_size[P1] = root_prob.size() - hand_size[P0];
    hand_card = vector<int>(cards.size());
    hand_hash = vector<size_t>(root_prob.size());
    int stop[N_PLAYER] = {hand_size[P0]<<1, cards.size()};
    int i = 0, j = 0, k = 0, n = 0;
    for(int p = 0; p < N_PLAYER; p++) {
        for(n = hand_size[p], i = j; j < stop[p]; j += 2, i++) {
            hand_card[i] = cards[j];
            hand_card[i+n] = cards[j+1];
            hand_hash[k++] = two_card_hash(cards[j], cards[j+1]);
        }
    }
    hand_card_ptr[P0] = hand_card.data();
    hand_card_ptr[P1] = hand_card_ptr[P0] + stop[P0];
    hand_hash_ptr[P0] = hand_hash.data();
    hand_hash_ptr[P1] = hand_hash_ptr[P0] + hand_size[P0];
}

void SliceCFR::init_hand_card(vector<PrivateCards> &range, vector<int> &cards, vector<float> &prob, size_t board, vector<PrivateCards> &out) {
    unordered_set<size_t> seen;
    for(PrivateCards &hand : range) {
        size_t hash = hand.toBoardLong();
        if(seen.count(hash)) continue;// 去重
        if(hash & board) continue;// 和公共牌冲突
        seen.insert(hash);
        cards.push_back(min(hand.card1, hand.card2));
        cards.push_back(max(hand.card1, hand.card2));
        prob.push_back(hand.weight);
        out.push_back(hand);
    }
}

void SliceCFR::init_same_hand_idx() {
    int n = root_prob.size(), p0_size = hand_size[P0];
    same_hand_idx = vector<int>(n, -1);
    unordered_map<size_t, int> hash2idx;
    for(int h = 0; h < p0_size; h++) hash2idx[hand_hash[h]] = h;
    for(int h = p0_size; h < n; h++) {// P1
        size_t hash = hand_hash[h];
        if(hash2idx.count(hash)) {
            same_hand_idx[h] = hash2idx[hash];
            same_hand_idx[hash2idx[hash]] = h - p0_size;
        }
    }
    same_hand_ptr[P0] = same_hand_idx.data();
    same_hand_ptr[P1] = same_hand_ptr[P0] + p0_size;
}

void SliceCFR::normalization() {
    int p0_size = hand_size[P0], n = root_prob.size();
    norm = 0;
    // 每个history的概率为p0_prob*p1_prob*chance_prob*mask/norm
    // p0手牌,p1手牌,公共牌之间有冲突时mask=0,无冲突时mask=1
    // cfr迭代过程中，不需要考虑norm
    for(int p0 = 0; p0 < p0_size; p0++) {
        for(int p1 = p0_size; p1 < n; p1++) {
            if(!cards_valid(hand_hash[p0], hand_hash[p1])) continue;
            norm += root_prob[p0] * root_prob[p1];
        }
    }
}

size_t SliceCFR::estimate_tree_size() {
    for(int i = 0; i < N_TYPE; i++) node_cnt[i] = 0;
    if(tree == nullptr) return 0;
    size_t size = _estimate_tree_size(tree->getRoot());
    n_leaf_node = node_cnt[FOLD_TYPE] + node_cnt[SHOWDOWN_TYPE];
    n_player_node = node_cnt[N_LEAF_TYPE+P0] + node_cnt[N_LEAF_TYPE+P1];
    size *= sizeof(float);
    size += n_leaf_node * sizeof(LeafNode);
    size += n_player_node * sizeof(Node);
    return size;
}

size_t SliceCFR::_estimate_tree_size(shared_ptr<GameTreeNode> node) {
    int type = node->getType(), round = GameTreeNode::gameRound2int(node->getRound()), n_act = 0;
    size_t size = 0;
    if(type == GameTreeNode::ACTION) {
        shared_ptr<ActionNode> act_node = dynamic_pointer_cast<ActionNode>(node);
        vector<shared_ptr<GameTreeNode>> children = act_node->getChildrens();
        n_act = children.size();
        int player = act_node->getPlayer();
        node_cnt[N_LEAF_TYPE + player]++;
        size += get_size(n_act, hand_size[player]);
        for(int i = 0; i < n_act; i++) size += _estimate_tree_size(children[i]);
    }
    else if(type == GameTreeNode::CHANCE) {
        shared_ptr<ChanceNode> chance_node = dynamic_pointer_cast<ChanceNode>(node);
        shared_ptr<GameTreeNode> children = chance_node->getChildren();// 不为null
        int child_type = children->getType();
        n_act = chance_branch[round] + 4;
        node_cnt[N_LEAF_TYPE + CHANCE_PLAYER]++;
        if(child_type == GameTreeNode::ACTION || child_type == GameTreeNode::SHOWDOWN) {
            for(int i = 0; i < n_act; i++) size += _estimate_tree_size(children);
        }
        else {// CHANCE之后接着CHANCE,再接着SHOWDOWN
            node_cnt[SHOWDOWN_TYPE] += (n_act*(n_act-1)>>1);
        }
    }
    else if(type == GameTreeNode::SHOWDOWN) node_cnt[SHOWDOWN_TYPE]++;
    else node_cnt[FOLD_TYPE]++;
    return size;
}

void SliceCFR::dfs(shared_ptr<GameTreeNode> node, int parent_act, int parent_dfs_idx, int parent_p0_act, int parent_p0_idx, int parent_p1_act, int parent_p1_idx, int cnt0, int cnt1, int info) {
    int curr_idx = dfs_idx++;
    int type = node->getType(), round = GameTreeNode::gameRound2int(node->getRound()), n_act = 0;
    if(type == GameTreeNode::ACTION) {
        shared_ptr<ActionNode> act_node = dynamic_pointer_cast<ActionNode>(node);
        ActionNode *p = act_node.get();
        int r_offset = round - init_round;
        if(node_idx.find(p) == node_idx.end()) node_idx[p] = vector<int>(combination_num[r_offset], -1);
        int j = decode_idx0(info), k = decode_idx1(info);
        if(r_offset == 0) {
            assert(j == -1 && k == -1);
            node_idx[p][0] = curr_idx;
        }
        else if(r_offset == 1) {
            assert(j != -1 && k == -1);
            node_idx[p][poss_card[j]] = curr_idx;
        }
        else {
            assert(r_offset == 2 && j != -1 && k != -1);
            node_idx[p][poss_card[j]*N_CARD+poss_card[k]] = curr_idx;
        }
        int player = act_node->getPlayer();
        vector<shared_ptr<GameTreeNode>> children = act_node->getChildrens();
        n_act = children.size();
        dfs_node.emplace_back(player, n_act, parent_act, info | round, parent_dfs_idx, parent_p0_act, parent_p0_idx, parent_p1_act, parent_p1_idx);
        vector<vector<int>> &player_slice = slice[player];
        if(player == P0) {
            if(player_slice.size() == cnt0) player_slice.emplace_back();
            player_slice[cnt0++].push_back(curr_idx);
            for(int i = 0; i < n_act; i++) dfs(children[i], i, curr_idx, i, curr_idx, parent_p1_act, parent_p1_idx, cnt0, cnt1, info);
        }
        else {// P1
            if(player_slice.size() == cnt1) player_slice.emplace_back();
            player_slice[cnt1++].push_back(curr_idx);
            for(int i = 0; i < n_act; i++) dfs(children[i], i, curr_idx, parent_p0_act, parent_p0_idx, i, curr_idx, cnt0, cnt1, info);
        }
    }
    else if(type == GameTreeNode::CHANCE) {
        shared_ptr<ChanceNode> chance_node = dynamic_pointer_cast<ChanceNode>(node);
        shared_ptr<GameTreeNode> children = chance_node->getChildren();// 不为null
        int child_type = children->getType();
        n_act = chance_branch[round] + 4;
        this->chance_node.push_back(curr_idx);
        if(child_type == GameTreeNode::ACTION || child_type == GameTreeNode::SHOWDOWN) {// 需要发1张牌
            dfs_node.emplace_back(CHANCE_PLAYER, n_act, parent_act, info | round, parent_dfs_idx, parent_p0_act, parent_p0_idx, parent_p1_act, parent_p1_idx);
            // 发牌信息编码,只有1张牌时,占用idx0,有2张牌时,占用idx0,idx1
            int j = decode_idx0(info), new_info = 0;
            for(int i = 0, k = 0; i < n_act; i++, k++) {// 动作索引i,poss_card索引k
                if(j == -1) new_info = code_idx0(k);// 第一次发牌
                else {// 第二次发牌,最多发两次牌
                    if(k == j) k++;// 两次选的一样,则第二次改成下一个
                    new_info = code_idx0(j) | code_idx1(k);
                }
                dfs(children, i, curr_idx, parent_p0_act, parent_p0_idx, parent_p1_act, parent_p1_idx, cnt0, cnt1, new_info);
            }
        }
        else {// CHANCE之后接着CHANCE,再接着SHOWDOWN,需要连续发2张牌
            assert(round == TURN_ROUND);
            shared_ptr<ChanceNode> child = dynamic_pointer_cast<ChanceNode>(children);
            assert(child->getChildren()->getType() == GameTreeNode::SHOWDOWN);
            int parent_player = dfs_node[parent_dfs_idx].player;// 父节点玩家
            dfs_node.emplace_back(CHANCE_PLAYER, n_act*(n_act-1)>>1, parent_act, info | round, parent_dfs_idx, parent_p0_act, parent_p0_idx, parent_p1_act, parent_p1_idx);
            // float val = node->getPot()/2*2/chance_den[RIVER_ROUND];
            float val = node->getPot()/chance_den[RIVER_ROUND];
            for(int i = 0, j = 0; j < n_act; j++) {
                for(int k = j+1; k < n_act; k++) {
                    ev[SHOWDOWN_TYPE].push_back(val);
                    leaf_node_dfs[SHOWDOWN_TYPE].push_back(dfs_idx++);
                    info = code_idx0(j) | code_idx1(k);
                    dfs_node.emplace_back(CHANCE_PLAYER, 0, i++, info, curr_idx, parent_p0_act, parent_p0_idx, parent_p1_act, parent_p1_idx);
                }
            }
        }
    }
    else {// river SHOWDOWN, fold
        assert(parent_dfs_idx != -1);
        int parent_player = dfs_node[parent_dfs_idx].player;// 父节点玩家
        int i = SHOWDOWN_TYPE;
        float val = 0;
        if(type == GameTreeNode::SHOWDOWN) val = node->getPot()/2;
        else {// fold
            vector<double> pot = dynamic_pointer_cast<TerminalNode>(node)->get_payoffs();
            val = pot[P0];
            i = FOLD_TYPE;
        }
        leaf_node_dfs[i].push_back(curr_idx);
        ev[i].push_back(val / chance_den[round]);
        dfs_node.emplace_back(parent_player, 0, parent_act, info, parent_dfs_idx, parent_p0_act, parent_p0_idx, parent_p1_act, parent_p1_idx);
    }
}

void SliceCFR::init_poss_card(Deck& deck, size_t board) {
    vector<Card> &cards = deck.getCards();
    for(Card& card : cards) {
        int i = card.getCardInt();
        if(cards_valid(1LL<<i, board)) poss_card.push_back(i);
    }
    // 3张公共牌已经发出，后面每次只发1张公共牌
    for(int r = init_round+1; r >= 0; r--) chance_branch[r] = poss_card.size() - 4;// 排除2个玩家的手牌，总共4张
    for(int r = init_round+2; r < N_ROUND; r++) chance_branch[r] = chance_branch[r-1] - 1;
    print_array(chance_branch, N_ROUND);
    for(int r = 0; r <= init_round; r++) chance_den[r] = 1;
    for(int r = init_round+1; r < N_ROUND; r++) chance_den[r] = chance_den[r-1] * chance_branch[r];
    print_array(chance_den, N_ROUND);
}

void SliceCFR::_reach_prob(int player, bool avg_strategy) {
    vector<int>& offset = slice_offset[player];
    int n = offset.size(), n_hand = hand_size[player];
    node_func func = avg_strategy ? reach_prob_avg : reach_prob;
    for(int i = 1; i < n; i++) {
        #pragma omp parallel for
        for(int j = offset[i-1]; j < offset[i]; j++) {
            func(player_node_ptr+j, n_hand);
        }
    }
}
void SliceCFR::_rm(int player, bool avg_strategy) {
    node_func func = avg_strategy ? rm_avg : rm;
    int s = slice_offset[player][0], e = slice_offset[player].back(), n_hand = hand_size[player];
    #pragma omp parallel for
    for(int i = s; i < e; i++) {
        func(player_node_ptr+i, n_hand);
    }
}

void SliceCFR::clear_data(int player) {
    int s = slice_offset[player][0], e = slice_offset[player].back(), n_hand = hand_size[player];
    size_t size = 0;
    for(int i = s; i < e; i++) {
        size = get_size(player_node_ptr[i].n_act, n_hand) * sizeof(float);
        memset(player_node_ptr[i].data, 0, size);
    }
}

void SliceCFR::clear_root_cfv() {
    size_t size = root_prob.size() * sizeof(float);
    memset(root_cfv_ptr[P0], 0, size);
}

bool SliceCFR::print_exploitability(int iter, Timer &timer) {
    vector<float> res = exploitability();
    logger->log("%d:%.3fs", iter, timer.ms()/1000.0);
    float avg = (res[0] + res[1]) / 2;
    logger->log("%d:%f %f %f", iter, res[0], res[1], avg);
    return avg <= tol;
}

void SliceCFR::train() {
    init();
    if(!init_succ) return;
    Timer timer;
    clear_data(P0);
    clear_data(P1);
    // _rm(P0, false);
    // _rm(P1, false);
    // _reach_prob(P0, false);
    print_exploitability(0, timer);
    // 计算exploitability后,双方的rm和p0的reach_prob已经恢复
    // pos_coef = neg_coef = coef = 0;
    double temp = 0;
    int cnt = 0, iter = 0;
    while(iter < steps) {
        temp = pow(iter, alpha);
        pos_coef = temp / (temp + 1);
        temp = pow(iter, beta);
        neg_coef = temp / (temp + 1);
        // neg_coef = 0.5;
        coef = pow((float)iter/(iter+1), gamma);

        clear_root_cfv();
        for(int player = P0; player < N_PLAYER; player++) {
            step(iter, player, CFR_TASK);
        }
        iter++;
        if((++cnt) == interval) {
            cnt = 0;
            if(print_exploitability(iter, timer)) break;
        }
        if(stop_flag) break;
    }
    if(cnt) {
        print_exploitability(iter, timer);
    }
    logger->log("collecting statics");
    for(int player = P0; player < N_PLAYER; player++) {
        _rm(1-player, true);
        step(iter, player, CFV_TASK);
    }
    cfv_to_ev();
    logger->log("statics collected");
}

// 执行更新任务时,player到达概率需要提前计算好
void SliceCFR::step(int iter, int player, int task) {
#ifdef TIME_LOG
    size_t start = timeSinceEpochMillisec(), end = 0;
#endif
    int opp = 1 - player, my_hand = hand_size[player];
    _reach_prob(opp, task != CFR_TASK);
#ifdef TIME_LOG
    end = timeSinceEpochMillisec();
    size_t t1 = end - start;
    start = end;
#endif

    leaf_cfv(player);
#ifdef TIME_LOG
    end = timeSinceEpochMillisec();
    size_t t2 = end - start;
    start = end;
#endif

    vector<int>& offset = slice_offset[player];
    if(task == CFR_TASK) {
        #pragma omp parallel for
        for(int j = offset[0]; j < offset.back(); j++) {
            discount_data(player_node_ptr+j, my_hand, pos_coef, neg_coef, coef);
        }
    }
#ifdef TIME_LOG
    end = timeSinceEpochMillisec();
    size_t t3 = end - start;
    start = end;
#endif
    
    node_func func;
    switch(task) {
        case EXP_TASK:{func = best_cfv_up;break;}
        case CFV_TASK:{func = cfv_up_avg;break;}
        default:func = cfv_up;
    }
    for(int i = offset.size()-1; i > 0; i--) {
        #pragma omp parallel for
        for(int j = offset[i-1]; j < offset[i]; j++) {
            func(player_node_ptr+j, my_hand);
        }
    }
#ifdef TIME_LOG
    end = timeSinceEpochMillisec();
    size_t t4 = end - start;
    printf("%zd\t%zd\t%zd\t%zd\n", t1, t2, t3, t4);
#endif
}

vector<float> SliceCFR::exploitability() {
    int opp = 0;
    clear_root_cfv();
    for(int player = P0; player < N_PLAYER; player++) {
#ifdef TIME_LOG
        size_t start = timeSinceEpochMillisec();
#endif
        opp = 1 - player;
        _rm(opp, true);// 改变对方策略
#ifdef TIME_LOG
        size_t t1 = timeSinceEpochMillisec() - start;
#endif
        step(0, player, EXP_TASK);
#ifdef TIME_LOG
        start = timeSinceEpochMillisec();
#endif
        _rm(opp, false);// 恢复对方策略
#ifdef TIME_LOG
        size_t t2 = timeSinceEpochMillisec() - start;
        printf("rm time:%zd\t%zd\n", t1, t2);
#endif
    }
    post_process();
    _reach_prob(P0, false);// 恢复P0的reach_prob,用于下一次迭代
    int m = 0, n = hand_size[P0];
    float ev0 = 0, ev1 = 0;
    for(int i = m; i < n; i++) ev0 += root_cfv[i] * root_prob[i];
    m = n; n = root_prob.size();
    for(int i = m; i < n; i++) ev1 += root_cfv[i] * root_prob[i];
    return {ev0/norm, ev1/norm};
}

void SliceCFR::stop() {
    stop_flag = true;
}
json SliceCFR::dumps(bool with_status, int depth) {// depth:max_round
    int idx = 0;
    json ans = reConvertJson(tree->getRoot(), 0, depth, idx, 0);
    if(idx != dfs_idx) throw runtime_error("dfs idx error");
    return std::move(ans);
}
vector<vector<vector<float>>> SliceCFR::get_strategy(shared_ptr<ActionNode> node, vector<Card> cards) {
    vector<vector<vector<float>>> ans(N_CARD, vector<vector<float>>(N_CARD));
    output_data(node.get(), cards, ans, false);
    return std::move(ans);
}
vector<vector<vector<float>>> SliceCFR::get_evs(shared_ptr<ActionNode> node, vector<Card> cards) {
    vector<vector<vector<float>>> ans(N_CARD, vector<vector<float>>(N_CARD));
    output_data(node.get(), cards, ans, true);
    return std::move(ans);
}
void SliceCFR::output_data(ActionNode *node, vector<Card> &cards, vector<vector<vector<float>>> &out, bool ev) {
    int r_offset = GameTreeNode::gameRound2int(node->getRound()) - init_round;
    if(cards.size() != r_offset || r_offset > 2) throw runtime_error("chance_cards error");
    int idx = 0;
    size_t board = init_board;
    if(r_offset >= 1) {
        idx = cards[0].getCardInt();
        board |= 1LL << idx;
    }
    if(r_offset == 2) {
        idx = idx * N_CARD + cards[1].getCardInt();
        board |= 1LL << cards[1].getCardInt();
    }
    vector<vector<float>> data;
    if(ev) data = get_ev(node_idx.at(node)[idx]);
    else data = get_avg_strategy(node_idx.at(node)[idx]);
    int player = node->getPlayer(), n_hand = hand_size[player], *card = hand_card_ptr[player];
    size_t *ptr = hand_hash_ptr[player];
    for(int h = 0; h < n_hand; h++) {
        if(!cards_valid(ptr[h], board)) continue;
        out[card[h]+min_card][card[h+n_hand]+min_card].swap(data[h]);
    }
}
vector<vector<float>> SliceCFR::get_ev(int idx) {
    Node &node = player_node[dfs_idx_map[idx]];
    int n_hand = hand_size[dfs_node[idx].player], n_act = node.n_act;
    int i = 0, h = 0, j = 0;
    float *cfv = node.data;
    vector<vector<float>> ev(n_hand, vector<float>(n_act));// [n_hand,n_act]
    for(j = 0; j < n_act; j++) {
        for(h = 0; h < n_hand; h++) ev[h][j] = cfv[i++];
    }
    return std::move(ev);
}
vector<vector<float>> SliceCFR::get_avg_strategy(int idx) {
    Node &node = player_node[dfs_idx_map[idx]];
    int n_hand = hand_size[dfs_node[idx].player], n_act = node.n_act;
    int size = n_act * n_hand, i = 0, h = 0, j = 0;
    float sum = 0, *strategy_sum = node.data + (size << 1), uni = 1.0 / n_act;
    vector<vector<float>> strategy(n_hand, vector<float>(n_act));// [n_hand,n_act]
    for(h = 0; h < n_hand; h++) {
        sum = 0;
        for(i = h; i < size; i += n_hand) sum += strategy_sum[i];
        if(sum == 0) {
            for(j = 0; j < n_act; j++) strategy[h][j] = uni;
        }
        else {
            for(j = 0, i = h; j < n_act; j++, i += n_hand) strategy[h][j] = strategy_sum[i] / sum;
        }
    }
    return std::move(strategy);
}
json SliceCFR::reConvertJson(const shared_ptr<GameTreeNode>& node, int depth, int max_depth, int &idx, int info) {
    int curr_idx = idx++;
    int type = node->getType(), n_act = 0;
    json ans;
    if(type == GameTreeNode::ACTION) {
        shared_ptr<ActionNode> one_node = dynamic_pointer_cast<ActionNode>(node);
        vector<string> actions_str;
        if(depth < max_depth) {
            int player = one_node->getPlayer();
            for(GameActions one_action : one_node->getActions()) actions_str.push_back(one_action.toString());
            ans["actions"] = actions_str;
            ans["player"] = player;
            ans["node_type"] = "action_node";
            
            vector<vector<float>> strategy = get_avg_strategy(curr_idx);
            ans["strategy"] = json();
            ans["strategy"]["actions"] = actions_str;
            json stt;
            size_t n_hand = hand_size[player];
            int *ptr = hand_card_ptr[player];
            for(size_t i = 0; i < n_hand; i++) {
                stt[Card::intCard2Str(ptr[i+n_hand]+min_card)+Card::intCard2Str(ptr[i]+min_card)] = strategy[i];
            }
            ans["strategy"]["strategy"] = std::move(stt);
            
            ans["childrens"] = json();
        }
        vector<shared_ptr<GameTreeNode>> children = one_node->getChildrens();
        n_act = children.size();
        for(int i = 0; i < n_act; i++) {
            json child = reConvertJson(children[i], depth, max_depth, idx, info);
            if(depth < max_depth) ans["childrens"][actions_str[i]] = child;
        }
    }
    else if(type == GameTreeNode::CHANCE) {
        if((++depth) <= max_depth) ans["node_type"] = "chance_node";
        shared_ptr<ChanceNode> chance_node = dynamic_pointer_cast<ChanceNode>(node);
        shared_ptr<GameTreeNode> children = chance_node->getChildren();// 不为null
        int child_type = children->getType();
        n_act = chance_branch[GameTreeNode::gameRound2int(node->getRound())] + 4;
        if(child_type == GameTreeNode::ACTION || child_type == GameTreeNode::SHOWDOWN) {// 需要发1张牌
            if(depth <= max_depth) ans["deal_number"] = n_act;
            if(depth < max_depth) ans["dealcards"] = json();// 需要展开子节点
            int j = decode_idx0(info), new_info = 0;
            for(int i = 0, k = 0; i < n_act; i++, k++) {// 动作索引i,poss_card索引k
                if(j == -1) new_info = code_idx0(k);// 第一次发牌
                else {// 第二次发牌,最多发两次牌
                    if(k == j) k++;// 两次选的一样,则第二次改成下一个
                }
                json child = reConvertJson(children, depth, max_depth, idx, new_info);
                if(depth < max_depth) ans["dealcards"][Card::intCard2Str(poss_card[k])] = child;
            }
        }
        else {
            n_act = n_act*(n_act-1)>>1;
            idx += n_act;
            if(depth <= max_depth) ans["deal_number"] = n_act;
        }
    }
    // else {}
    return std::move(ans);
}