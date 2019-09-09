#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <arm_neon.h>

float NeonMaximum(const float* in, const size_t size) {
  const float* in_data = in;
  const float* in_data_end = in + size;
  float32x4_t max = vdupq_n_f32(-std::numeric_limits<float>::max());
  for (; in_data + 4 <= in_data_end; in_data += 4) {
    const float32x4_t v1 = vld1q_f32(
        reinterpret_cast<const float32_t *>(in_data));
    max = vmaxq_f32(v1, max);
  }

  // Get the maximum float among the 4 floats in the SIMD register.
  float32x2_t max1 = vget_low_f32(max);
  float32x2_t max2 = vget_high_f32(max);
  max1 = vmax_f32(max1, max2);
  float ret_max = (vget_lane_f32(max1, 0) > vget_lane_f32(max1, 1)) ?
      vget_lane_f32(max1, 0) : vget_lane_f32(max1, 1);

  // Get the maximum among the ret_max and the rest floats in the input array
  // which couldn't be fitted into 4-bytes SIMD register.
  for (; in_data < in_data_end; ++in_data) {
    if (ret_max < *in_data) {
      ret_max = *in_data;
    }
  }
  return ret_max;
}

float Maximum(const float* in, const size_t size) {
  const float* in_data = in;
  const float* in_data_end = in + size;
  float ret_max = -std::numeric_limits<float>::max();
  for (; in_data < in_data_end; ++in_data) {
    if (ret_max < *in_data) {
      ret_max = *in_data;
    }
  }
  return ret_max;
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
  std::cout << "Slow Maximum : " << Maximum(v, num)
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
  start = std::clock();
  std::cout << "Neon Maximum : " << NeonMaximum(v, num)
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
}
