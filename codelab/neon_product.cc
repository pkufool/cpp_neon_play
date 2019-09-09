#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <arm_neon.h>

float SIMDRegisterSum(const float32x4_t sum) {
  float32x2_t sum1 = vget_low_f32(sum);
  float32x2_t sum2 = vget_high_f32(sum);
  sum1 = vadd_f32(sum1, sum2);
  return vget_lane_f32(sum1, 0) + vget_lane_f32(sum1, 1);
}

float NeonProduct(const float* v1, const float* v2, const int size) {
  float32x4_t sum1 = vdupq_n_f32(0);
  float32x4_t sum2 = vdupq_n_f32(0);
  int i = 0;
  int max_offset = size - 8;
  while (i <= max_offset) {
    const float32x4_t a1 = vld1q_f32(v1 + i);
    const float32x4_t a2 = vld1q_f32(v1 + i + 4);
    const float32x4_t b1 = vld1q_f32(v2 + i);
    const float32x4_t b2 = vld1q_f32(v2 + i + 4);
    sum1 = vmlaq_f32(sum1, a1, b1);
    sum2 = vmlaq_f32(sum2, a2, b2);
    i += 8;
  }

  sum1 = vaddq_f32(sum1, sum2);
  float ret_sum = SIMDRegisterSum(sum1);

  // Process the remaining elements.
  for (; i < size; ++i) {
    ret_sum += v1[i] * v2[i];
  }
  return ret_sum;
}

float Product(const float* v1, const float* v2, const int size) {
  float sum = 0;
  for (int i = 0; i < size; ++i) {
    sum += v1[i] * v2[i];
  }
  return sum;
} 

int main() {
  int num = 1000;
  float v1[num], v2[num];
  std::default_random_engine e;
  std::uniform_real_distribution<float> u(0, 1);
  for (int i = 0; i < num; ++i) {
    v1[i] = u(e);
    v2[i] = u(e);
  }
  std::clock_t start;
  start = std::clock();
  std::cout << "Slow Product : " << Product(v1, v2, num)
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
  start = std::clock();
  std::cout << "Neon Product : " << NeonProduct(v1, v2, num)
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
}

