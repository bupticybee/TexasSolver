#include "solver/cuda_cfr.h"
#include "solver/cuda_func.h"
#include "ranges/RiverRangeManager.h"

void cuda_error(cudaError_t error, const char *file, int line) {
    if(error != cudaSuccess) {
        printf("%s in %s at line %d\n", cudaGetErrorString(error), file, line);
        exit(EXIT_FAILURE);
    }
}

#define CHECK_ERROR(error) (cuda_error(error, __FILE__, __LINE__))

template<class T>
void copy_to_device(T *dev, T *host, int n, bool print=false) {
    if(!dev || !host || n <= 0) return;
    size_t size = n * sizeof(T);
    CHECK_ERROR(cudaMemcpy(dev, host, size, cudaMemcpyHostToDevice));
    if(!print) return;
    print_data(host, n);
    print_data_kernel<<<1, 1>>>(dev, n);
    cudaDeviceSynchronize();
}

int max_malloc_len(int left, int right, int group_size = 1) {
    int mid = 0, size = group_size * sizeof(float);
    float *p = nullptr;
    while(left < right) {
        mid = (left + right + 1) >> 1;// 靠右
        if(cudaMalloc(&p, mid * size) == cudaSuccess) {
            cudaFree(p);
            left = mid;
        }
        else right = mid - 1;
    }
    return left;
}

void CudaCFR::leaf_cfv(int player) {
    Timer timer;
    int opp = 1 - player, offset = player == P0 ? 0 : hand_size[P0];
    int my_hand = hand_size[player], opp_hand = hand_size[opp];
    int size = node_cnt[FOLD_TYPE];
    int block = block_size(size);
    clear_prob_sum(size);
    fold_cfv_kernel<<<block, LANE_SIZE>>>(
        player, size, dev_leaf_node, dev_prob_sum, my_hand, opp_hand,
        dev_hand_card_ptr[opp], dev_hand_hash_ptr[opp], dev_same_hand_idx+offset
    );
    cudaDeviceSynchronize();
    // printf("fold_cfv:%zd ms\n", timer.ms(true));
    
    size = node_cnt[SHOWDOWN_TYPE];
    block = block_size(size);
    clear_prob_sum(size);
    sd_cfv_kernel<<<block, LANE_SIZE>>>(
        player, size, dev_leaf_node+sd_offset, dev_prob_sum, my_hand, opp_hand,
        dev_hand_card_ptr[player], dev_hand_card_ptr[opp], n_card
    );
    cudaDeviceSynchronize();
    // printf("sd_cfv:%zd ms\n", timer.ms());
}

CudaCFR::~CudaCFR() {
    if(dev_root_cfv) CHECK_ERROR(cudaFree(dev_root_cfv));
    if(dev_hand_card) CHECK_ERROR(cudaFree(dev_hand_card));
    if(dev_hand_hash) CHECK_ERROR(cudaFree(dev_hand_hash));
    if(dev_nodes) CHECK_ERROR(cudaFree(dev_nodes));
    if(dev_leaf_node) CHECK_ERROR(cudaFree(dev_leaf_node));
    for(float *p : dev_data) {
        if(p) CHECK_ERROR(cudaFree(p));
    }
    if(dev_prob_sum) CHECK_ERROR(cudaFree(dev_prob_sum));
    for(int *p : dev_strength) {
        if(p) CHECK_ERROR(cudaFree(p));
    }
}

void CudaCFR::set_cfv_and_offset(DFSNode &node, int player, float *&cfv, int &offset) {
    if(player == -1) player = node.player;// 向上连接同玩家节点
    int p_idx = node.parent_p0_idx, act_idx = node.parent_p0_act;// 向上连接P0
    if(player != P0) {// 向上连接P1
        p_idx = node.parent_p1_idx;
        act_idx = node.parent_p1_act;
    }
    if(p_idx == -1) {
        cfv = root_cfv_ptr[player];
        offset = root_prob_ptr[player] - root_cfv_ptr[player];
    }
    else {
        if(player != dfs_node[p_idx].player) throw runtime_error("player mismatch");
        cfv = dev_data[dfs_idx_map[p_idx]] + cfv_offset(hand_size[player], act_idx);
        offset = reach_prob_to_cfv(dfs_node[p_idx].n_act, hand_size[player]);
    }
}

size_t CudaCFR::init_player_node() {
    size_t total = 0, size = 0, node_size = n_player_node * sizeof(Node);
    vector<Node> cpu_node(n_player_node);// 与cuda内存对应
    CHECK_ERROR(cudaMalloc(&dev_nodes, node_size));
    total += node_size;
    dev_data = vector<float*>(n_player_node, nullptr);
    dfs_idx_map = vector<int>(dfs_idx, -1);
    slice_offset = vector<vector<int>>(N_PLAYER);
    int mem_idx = 0;
    for(int i = 0; i < N_PLAYER; i++) {// 枚举player
        for(vector<int> &nodes : slice[i]) {// 枚举slice
            slice_offset[i].push_back(mem_idx);
            for(int idx : nodes) {// 枚举node
                DFSNode &node = dfs_node[idx];
                Node &target = cpu_node[mem_idx];// cpu存储位置
                target.n_act = node.n_act;
                set_cfv_and_offset(node, -1, target.parent_cfv, target.parent_offset);
                size = get_size(node.n_act, hand_size[node.player]) * sizeof(float);
                CHECK_ERROR(cudaMalloc(&target.data, size));
                if(target.data == nullptr) throw runtime_error("malloc error");
                total += size;
                dev_data[mem_idx] = target.data;
                dfs_idx_map[idx] = mem_idx++;
            }
        }
        slice_offset[i].push_back(mem_idx);
    }
    CHECK_ERROR(cudaMemcpy(dev_nodes, cpu_node.data(), node_size, cudaMemcpyHostToDevice));
    return total;
}

size_t CudaCFR::init_leaf_node() {
    size_t node_size = n_leaf_node * sizeof(CudaLeafNode);
    vector<CudaLeafNode> cpu_node(n_leaf_node);// 与cuda内存对应
    CHECK_ERROR(cudaMalloc(&dev_leaf_node, node_size));
    int mem_idx = 0;
    for(int t = 0; t < N_LEAF_TYPE; t++) {
        for(int i = 0; i < leaf_node_dfs[t].size(); i++) {
            DFSNode &node = dfs_node[leaf_node_dfs[t][i]];
            CudaLeafNode &target = cpu_node[mem_idx++];// cpu存储位置
            target.val = ev[t][i];
            target.offset_prob_sum = i * n_card;
            set_cfv_and_offset(node, P0, target.data_p0, target.offset_p0);
            set_cfv_and_offset(node, P1, target.data_p1, target.offset_p1);
            int j = decode_idx0(node.info), k = decode_idx1(node.info);
            size_t info = init_board;
            if(t == FOLD_TYPE) {
                if(j != -1) info |= 1LL << poss_card[j];
                if(k != -1) info |= 1LL << poss_card[k];
                target.info = (int *)info;
            }
            else {
                if(j == -1) info = 0;
                else if(k == -1) info = j;
                else info = tril_idx(j, k);
                target.info = dev_strength[info];
            }
        }
    }
    CHECK_ERROR(cudaMemcpy(dev_leaf_node, cpu_node.data(), node_size, cudaMemcpyHostToDevice));
    sd_offset = leaf_node_dfs[FOLD_TYPE].size();
    ev.clear();
    return node_size;
}

size_t CudaCFR::init_memory() {
    size_t total = 0;
    int n = root_prob.size();
    root_cfv = vector<float>(n, 0);
    size_t size = ((n << 1)) * sizeof(float);// cfv + prob
    CHECK_ERROR(cudaMalloc(&dev_root_cfv, size));
    total += size;
    root_cfv_ptr[P0] = dev_root_cfv;
    root_cfv_ptr[P1] = dev_root_cfv + hand_size[P0];
    root_prob_ptr[P0] = root_cfv_ptr[P0] + n;
    root_prob_ptr[P1] = root_prob_ptr[P0] + hand_size[P0];
    clear_root_cfv();
    copy_to_device(root_prob_ptr[P0], root_prob.data(), n);

    vector<int> temp_hand_card = hand_card;
    vector<size_t> temp_hand_hash = hand_hash;
    // [P0,P1,P0]
    temp_hand_card.insert(temp_hand_card.end(), hand_card.begin(), hand_card.begin()+(hand_size[P0]<<1));
    temp_hand_hash.insert(temp_hand_hash.end(), hand_hash.begin(), hand_hash.begin()+hand_size[P0]);
    n = temp_hand_card.size();
    size = (n + same_hand_idx.size()) * sizeof(int);// [P0,P1,P0] + [P0,P1]
    CHECK_ERROR(cudaMalloc(&dev_hand_card, size));
    total += size;
    copy_to_device(dev_hand_card, temp_hand_card.data(), n);
    dev_same_hand_idx = dev_hand_card + n;
    copy_to_device(dev_same_hand_idx, same_hand_idx.data(), same_hand_idx.size());
    
    n = temp_hand_hash.size();
    size = n * sizeof(size_t);
    CHECK_ERROR(cudaMalloc(&dev_hand_hash, size));
    total += size;
    copy_to_device(dev_hand_hash, temp_hand_hash.data(), n);
    dev_hand_card_ptr[P0] = dev_hand_card;
    dev_hand_card_ptr[P1] = dev_hand_card + (hand_size[P0]<<1);
    dev_hand_hash_ptr[P0] = dev_hand_hash;
    dev_hand_hash_ptr[P1] = dev_hand_hash + hand_size[P0];
    
    total += init_player_node();
    total += init_strength_table();
    total += init_leaf_node();
    
    // FOLD_TYPE,SHOWDOWN_TYPE,共用dev_prob_sum
    int len = max(node_cnt[FOLD_TYPE], node_cnt[SHOWDOWN_TYPE]);
    size = len * n_card * sizeof(float);
    CHECK_ERROR(cudaMalloc(&dev_prob_sum, size));
    total += size;
    return total;
}

size_t CudaCFR::init_strength_table() {
    SliceCFR::init_strength_table();
    int n = strength.size();
    size_t total = 0, size = 0;
    dev_strength = vector<int*>(n, nullptr);
    for(int i = 0; i < n; i++) {
        const RiverCombs *p0_comb = strength[i][P0].data, *p1_comb = strength[i][P1].data;
        int p0_size = strength[i][P0].size, p1_size = strength[i][P1].size, d = 0;
        vector<int> data(2+((p0_size+p1_size)<<1));
        data[d++] = 2 + (p0_size<<1);
        data[d++] = data.size();
        for(int j = 0; j < p0_size; j++) {
            data[d++] = p0_comb[j].rank;
            data[d++] = p0_comb[j].reach_prob_index;
        }
        for(int j = 0; j < p1_size; j++) {
            data[d++] = p1_comb[j].rank;
            data[d++] = p1_comb[j].reach_prob_index;
        }
        size = data.size() * sizeof(int);
        CHECK_ERROR(cudaMalloc(&dev_strength[i], size));
        total += size;
        copy_to_device(dev_strength[i], data.data(), data.size());
    }
    strength.clear();
    rrm.clear();
    return total;
}

size_t CudaCFR::estimate_tree_size() {
    for(int i = 0; i < N_TYPE; i++) node_cnt[i] = 0;
    if(tree == nullptr) return 0;
    size_t size = _estimate_tree_size(tree->getRoot());
    n_leaf_node = node_cnt[FOLD_TYPE] + node_cnt[SHOWDOWN_TYPE];
    n_player_node = node_cnt[N_LEAF_TYPE+P0] + node_cnt[N_LEAF_TYPE+P1];
    size *= sizeof(float);
    size += n_leaf_node * sizeof(CudaLeafNode);
    size += n_player_node * sizeof(Node);
    size += max(node_cnt[FOLD_TYPE], node_cnt[SHOWDOWN_TYPE]) * n_card * sizeof(float);
    return size;
}

void CudaCFR::_reach_prob(int player, bool avg_strategy) {
    vector<int>& offset = slice_offset[player];
    int n = offset.size() - 1, size = 0, block = 0, n_hand = hand_size[player];
    for(int i = 0; i < n; i++) {
        size = offset[i+1] - offset[i];
        block = block_size(size);
        if(avg_strategy) reach_prob_avg_kernel<<<block, LANE_SIZE>>>(dev_nodes+offset[i], size, n_hand);
        else reach_prob_kernel<<<block, LANE_SIZE>>>(dev_nodes+offset[i], size, n_hand);
        cudaDeviceSynchronize();
    }
}

void CudaCFR::_rm(int player, bool avg_strategy) {
    int size = node_cnt[N_LEAF_TYPE + player];
    int block = block_size(size);
    Node *node = dev_nodes + slice_offset[player][0];
    if(avg_strategy) rm_avg_kernel<<<block, LANE_SIZE>>>(node, size, hand_size[player]);
    else rm_kernel<<<block, LANE_SIZE>>>(node, size, hand_size[player]);
    cudaDeviceSynchronize();
}

void CudaCFR::clear_data(int player) {
    int size = node_cnt[N_LEAF_TYPE + player];
    int block = block_size(size);
    clear_data_kernel<<<block, LANE_SIZE>>>(dev_nodes+slice_offset[player][0], size, hand_size[player]);
    cudaDeviceSynchronize();
}

void CudaCFR::clear_prob_sum(int len) {
    CHECK_ERROR(cudaMemset(dev_prob_sum, 0, len * n_card * sizeof(float)));
    cudaDeviceSynchronize();
}

void CudaCFR::clear_root_cfv() {
    CHECK_ERROR(cudaMemset(dev_root_cfv, 0, root_cfv.size() * sizeof(float)));
    cudaDeviceSynchronize();
}

void CudaCFR::step(int iter, int player, int task) {
    Timer timer;
    int opp = 1 - player, my_hand = hand_size[player], size = 0, block = 0;
    _reach_prob(opp, task != CFR_TASK);
    size_t t1 = timer.ms(true);

    leaf_cfv(player);
    size_t t2 = timer.ms(true);

    if(task == CFR_TASK) {
        size = n_player_node;
        block = block_size(size);
        discount_data_kernel<<<block, LANE_SIZE>>>(dev_nodes, size, my_hand, pos_coef, neg_coef, coef);
        cudaDeviceSynchronize();
    }
    size_t t3 = timer.ms(true);
    vector<int>& offset = slice_offset[player];
    for(int i = offset.size()-2; i >= 0; i--) {
        size = offset[i+1] - offset[i];
        block = block_size(size);
        if(task == EXP_TASK) best_cfv_kernel<<<block, LANE_SIZE>>>(dev_nodes+offset[i], size, my_hand);
        else cfv_kernel<<<block, LANE_SIZE>>>(dev_nodes+offset[i], size, my_hand);
        cudaDeviceSynchronize();
    }
    size_t t4 = timer.ms();
    printf("%zd\t%zd\t%zd\t%zd\n", t1, t2, t3, t4);
}

void CudaCFR::post_process() {
    int n = root_cfv.size();
    CHECK_ERROR(cudaMemcpy(root_cfv.data(), dev_root_cfv, n * sizeof(float), cudaMemcpyDeviceToHost));
    // print_data(root_cfv.data(), n);
    // print_data_kernel<<<1, 1>>>(dev_root_cfv, n);
    // cudaDeviceSynchronize();
}

vector<vector<float>> CudaCFR::get_avg_strategy(int idx) {
    DFSNode &node = dfs_node[idx];
    int n_hand = hand_size[node.player], n_act = node.n_act;
    int size = n_act * n_hand, i = 0, h = 0, j = 0;
    float *dev = dev_data[dfs_idx_map[idx]] + (size << 1), sum = 0, uni = 1.0 / n_act;
    vector<float> strategy_sum(size);// [n_act,n_hand]
    CHECK_ERROR(cudaMemcpy(strategy_sum.data(), dev, size * sizeof(float), cudaMemcpyDeviceToHost));
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
    return strategy;
}
vector<vector<float>> CudaCFR::get_ev(int idx) {
    return {};
}
void CudaCFR::cfv_to_ev() {}
