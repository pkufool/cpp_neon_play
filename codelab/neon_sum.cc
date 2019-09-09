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

float NeonSum(const float* in, const size_t size) {
  float32x4_t sum = vdupq_n_f32(0);
  const float* in_data = in;
  const float* const in_data_end = in + size;
  for (; in_data + 4 <= in_data_end; in_data += 4) {
    const float32x4_t v1 = vld1q_f32(
        reinterpret_cast<const float32_t *>(in_data));
    sum = vaddq_f32(v1, sum);
  }

  float ret_sum = SIMDRegisterSum(sum);

  for (; in_data < in_data_end; ++in_data) {
    ret_sum += *in_data;
  }
  return ret_sum;
}

float Sum(const float* in, const size_t size) {
  float sum = 0.0f;
  for (size_t i = 0; i < size; ++i) {
    sum += in[i];
  }
  return sum;
}

int main() {
  int num = 1000;
  float v[num];
  std::default_random_engine e;
  std::uniform_real_distribution<float> u(0, 1);
  for (int i = 0; i < num; ++i) {
    v[i] = u(e);
  }
  std::clock_t start;
  start = std::clock();
  std::cout << "Slow Sum : " << Sum(v, num)
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
  start = std::clock();
  std::cout << "Neon Sum : " << NeonSum(v, num)
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
}

