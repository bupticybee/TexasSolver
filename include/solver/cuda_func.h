#ifndef _CUDA_FUNC_H_
#define _CUDA_FUNC_H_

#include "cuda_runtime.h"

extern __host__ __device__ void print_data(int *arr, int n);
extern __host__ __device__ void print_data(size_t *arr, int n);
extern __host__ __device__ void print_data(float *arr, int n);
extern __global__ void print_data_kernel(int *arr, int n);
extern __global__ void print_data_kernel(size_t *arr, int n);
extern __global__ void print_data_kernel(float *arr, int n);
extern __global__ void clear_data_kernel(Node *node, int size, int n_hand);
extern __global__ void rm_avg_kernel(Node *node, int size, int n_hand);
extern __global__ void rm_kernel(Node *node, int size, int n_hand);
extern __global__ void reach_prob_avg_kernel(Node *node, int size, int n_hand);
extern __global__ void reach_prob_kernel(Node *node, int size, int n_hand);
extern __global__ void fold_cfv_kernel(int player, int size, CudaLeafNode *node, float *opp_prob_sum, int my_hand, int opp_hand, int *hand_card, size_t *hand_hash, int *same_hand_idx);
extern __global__ void sd_cfv_kernel(int player, int size, CudaLeafNode *node, float *opp_prob_sum, int my_hand, int opp_hand, int *my_card, int *opp_card, int n_card);
extern __global__ void best_cfv_kernel(Node *node, int size, int n_hand);
extern __global__ void cfv_kernel(Node *node, int size, int n_hand);
extern __global__ void discount_data_kernel(Node *node, int size, int n_hand, float pos_coef, float neg_coef, float coef);

#endif // _CUDA_FUNC_H_