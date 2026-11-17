#include "solver/cuda_cfr.h"
#include "solver/cuda_func.h"
#include "device_launch_parameters.h"

__host__ __device__ void print_data(int *arr, int n) {
    if(arr != nullptr && n > 0) {
        printf("%d", arr[0]);
        for(int i = 1; i < n; i++) printf(",%d", arr[i]);
    }
    printf("\n");
}
__host__ __device__ void print_data(size_t *arr, int n) {
    if(arr != nullptr && n > 0) {
        printf("%llx", arr[0]);
        for(int i = 1; i < n; i++) printf(",%llx", arr[i]);
    }
    printf("\n");
}
__host__ __device__ void print_data(float *arr, int n) {
    if(arr != nullptr && n > 0) {
        printf("%.2f", arr[0]);
        for(int i = 1; i < n; i++) printf(",%.2f", arr[i]);
    }
    printf("\n");
}
__global__ void print_data_kernel(int *arr, int n) {
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i == 0) print_data(arr, n);
}
__global__ void print_data_kernel(size_t *arr, int n) {
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i == 0) print_data(arr, n);
}
__global__ void print_data_kernel(float *arr, int n) {
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i == 0) print_data(arr, n);
}

__global__ void clear_data_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    size = get_size(node->n_act, n_hand);
    float *data = node->data;
    for(i = 0; i < size; i++) data[i] = 0;
}

// 不同节点之间独立
__global__ void rm_avg_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    size = node->n_act * n_hand;
    int h = 0, sum_offset = size << 1;
    float *data = node->data + (size << 1);// strategy_sum
    float sum = 0;
    for(h = 0; h < n_hand; h++) {
        sum = 0;
        for(i = h; i < size; i += n_hand) sum += data[i];
        data[sum_offset+h] = sum;
    }
}
__global__ void rm_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    size = node->n_act * n_hand;
    int h = 0, sum_offset = size * 3;
    float *data = node->data + size;// regret_sum
    float sum = 0;
    for(h = 0; h < n_hand; h++) {
        sum = 0;
        for(i = h; i < size; i += n_hand) sum += max(0.0f, data[i]);
        data[sum_offset+h] = sum;
    }
}

// 上层slice传递到本层slice
__global__ void reach_prob_avg_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    int n_act = node->n_act;
    size = n_act * n_hand;
    int h = 0, sum_offset = size << 1;
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
__global__ void reach_prob_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    int n_act = node->n_act;
    size = n_act * n_hand;
    int h = 0, rp_offset = size << 1, sum_offset = rp_offset + size;
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

// 叶子节点向上层slice聚合,调用前需要清零上层slice的cfv
// same_hand_idx:player same_hand_idx
// hand_hash,hand_card:init opp   [P0,P1,P0]
__global__ void fold_cfv_kernel(int player, int size, CudaLeafNode *node, float *opp_prob_sum, int my_hand, int opp_hand, int *hand_card, size_t *hand_hash, int *same_hand_idx) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    opp_prob_sum += node->offset_prob_sum;
    size_t board = (size_t)node->info;
    float *cfv = nullptr, *opp_reach = nullptr, val = node->val;
    float prob_sum = 0, temp = 0;
    if(player == P0) {
        cfv = node->data_p0, opp_reach = node->data_p1 + node->offset_p1;
    }
    else {
        cfv = node->data_p1, opp_reach = node->data_p0 + node->offset_p0;
        val = -val;
    }
    for(i = 0; i < opp_hand; i++) {
        if(hand_hash[i] & board) continue;// 对方手牌与公共牌冲突
        temp = opp_reach[i];
        opp_prob_sum[hand_card[i]] += temp;// card1
        opp_prob_sum[hand_card[i+opp_hand]] += temp;// card2
        prob_sum += temp;
    }
    hand_hash += opp_hand;// ptr of player
    hand_card += (opp_hand << 1);
    for(i = 0; i < my_hand; i++) {
        if(hand_hash[i] & board) {
            // cfv[i] = 0;// 与公共牌冲突，cfv为0
            continue;
        }
        temp = same_hand_idx[i] != -1 ? opp_reach[same_hand_idx[i]] : 0;// 重复计算的部分
        temp = (prob_sum - opp_prob_sum[hand_card[i]] - opp_prob_sum[hand_card[i+my_hand]] + temp) * val;
        atomicAdd(cfv+i, temp);
    }
}

// showdown
__global__ void sd_cfv_kernel(int player, int size, CudaLeafNode *node, float *opp_prob_sum, int my_hand, int opp_hand, int *my_card, int *opp_card, int n_card) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;// 总任务数
    node += i;
    opp_prob_sum += node->offset_prob_sum;
    float *cfv = nullptr, *opp_reach = nullptr;
    float prob_sum = 0, temp = 0;
    int j = 0, size_j = 0, h = 0, s = 0, *strength_data = node->info;
    // strength_data:2+size0,2+size0+size1,sorted_data
    // i,size for player
    // j,size_j for opp
    if(player == P0) {
        i = 2, size_j = strength_data[1];
        size = j = strength_data[0];
        cfv = node->data_p0, opp_reach = node->data_p1 + node->offset_p1;
    }
    else {
        j = 2, size = strength_data[1];
        size_j = i = strength_data[0];
        cfv = node->data_p1, opp_reach = node->data_p0 + node->offset_p0;
    }
    // strength_data += 2;
    for(; i < size; i += 2) {// strength值变小,己方手牌变强
        s = strength_data[i];
        for(; j < size_j && strength_data[j] > s; j += 2) {// (胜过对方条件下)找到对方的最强手牌
            h = strength_data[j+1];
            temp = opp_reach[h];
            opp_prob_sum[opp_card[h]] += temp;// card1
            opp_prob_sum[opp_card[h+opp_hand]] += temp;// card2
            prob_sum += temp;
        }
        h = strength_data[i+1];
        temp = (prob_sum - opp_prob_sum[my_card[h]] - opp_prob_sum[my_card[h+my_hand]]) * node->val;
        atomicAdd(cfv+h, temp);
    }
    prob_sum = 0;
    for(h = 0; h < n_card; h++) opp_prob_sum[h] = 0;
    i -= 2, j -= 2;
    if(player == P0) {
        size_j = size;
        size = 2;
    }
    else {
        size = size_j;
        size_j = 2;
    }
    for(; i >= size; i -= 2) {// strength值变大,己方手牌变弱
        s = strength_data[i];
        for(; j >= size_j && strength_data[j] < s; j -= 2) {// (败给对方条件下)找到对方的最弱手牌
            h = strength_data[j+1];
            temp = opp_reach[h];
            opp_prob_sum[opp_card[h]] += temp;// card1
            opp_prob_sum[opp_card[h+opp_hand]] += temp;// card2
            prob_sum += temp;
        }
        h = strength_data[i+1];
        temp = (opp_prob_sum[my_card[h]] + opp_prob_sum[my_card[h+my_hand]] - prob_sum) * node->val;
        atomicAdd(cfv+h, temp);
    }
}

// 本层slice向上层slice聚合,上层cfv需要先清零
// 子节点cfv中选最大值
__global__ void best_cfv_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    size = node->n_act * n_hand;
    int h = 0;
    float *parent_cfv = node->parent_cfv, *cfv = node->data, val = 0;
    for(h = 0; h < n_hand; h++) {
        val = cfv[h];// 第一个
        for(i = h+n_hand; i < size; i += n_hand) val = max(val, cfv[i]);
        atomicAdd(parent_cfv+h, val);
    }
    for(i = 0; i < size; i++) cfv[i] = 0;// 清零cfv
}
// 子节点cfv加权求和
__global__ void cfv_kernel(Node *node, int size, int n_hand) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    int n_act = node->n_act;
    size = n_act * n_hand;
    int h = 0, sum_offset = size << 2;
    float *parent_cfv = node->parent_cfv, *cfv = node->data, val = 0;
    float *regret_sum = cfv + size;
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
        atomicAdd(parent_cfv+h, val);
        for(i = h; i < size; i += n_hand) regret_sum[i] += cfv[i] - val;// 更新regret_sum
        val = 0;
        for(i = h; i < size; i += n_hand) val += max(0.0f, regret_sum[i]);
        cfv[sum_offset+h] = val;// 求和
    }
    for(i = 0; i < size; i++) cfv[i] = 0;// 清零cfv
}

__global__ void discount_data_kernel(Node *node, int size, int n_hand, float pos_coef, float neg_coef, float coef) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i >= size) return;
    node += i;
    size = node->n_act * n_hand;
    float *regret_sum = node->data + size, *strategy_sum = regret_sum + size;
    for(i = 0; i < size; i++) {
        regret_sum[i] *= regret_sum[i] > 0 ? pos_coef : neg_coef;
        strategy_sum[i] = strategy_sum[i] * coef + strategy_sum[size+i];
    }
}
