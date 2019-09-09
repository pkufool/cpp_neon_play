#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <arm_neon.h>

// out = alpha * v1 * v2 + beta * out
void NeonAddVecVec(const float alpha, const float* v1, const float* v2,
                   const float beta, float* out, const size_t size) {
  float* out_data = out;
  int i = 0;
  int max_offset = size - 8;
  while (i <= max_offset) {
    float32x4_t a1 = vld1q_f32(v1 + i);
    float32x4_t a2 = vld1q_f32(v1 + i + 4);
    float32x4_t b1 = vld1q_f32(v2 + i);
    float32x4_t b2 = vld1q_f32(v2 + i + 4);
    a1 = vmulq_f32(a1, b1);
    a2 = vmulq_f32(a2, b2);
    b1 = vld1q_f32(out + i);
    b2 = vld1q_f32(out + i + 4);
    b1 = vmulq_n_f32(b1, beta);
    b2 = vmulq_n_f32(b2, beta);
    a1 = vmulq_n_f32(a1, alpha);
    a2 = vmulq_n_f32(a2, alpha);
    a1 = vaddq_f32(a1, b1);
    a2 = vaddq_f32(a2, b2);
    vst1q_f32(out_data + i, a1);
    vst1q_f32(out_data + i + 4, a2);
    i += 8;
  }
  for (; i < size; ++i) {
    out_data[i] = alpha * v1[i] * v2[i] + beta * out_data[i];
  }
}

void AddVecVec(const float alpha, const float* v1, const float* v2,
               const float beta, float* out, const size_t size) {
  for (int i = 0; i < size; ++i) {
    out[i] = alpha * v1[i] * v2[i] + beta * out[i];
  }
}

int main() {
  int num = 1000;
  float v1[num], v2[num], out1[num], out2[num];
  std::default_random_engine e;
  std::uniform_real_distribution<float> u(0, 1);
  for (int i = 0; i < num; ++i) {
    v1[i] = u(e);
    v2[i] = u(e);
    out1[i] = u(e);
    out2[i] = out1[i];
  }
  std::clock_t start;
  start = std::clock();
  AddVecVec(0.18, v1, v2, 2.01, out1, num);
  std::cout << "Slow AddVecVec"
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
  start = std::clock();
  NeonAddVecVec(0.18, v1, v2, 2.01, out2, num);
  std::cout << "Neon AddVecVec"
      << " use time : " << (std::clock() - start) / (double)CLOCKS_PER_SEC * 1e6
      << "." << std::endl;
  for (int i = 0; i < num; ++i) {
    if (abs(out1[i] - out2[i]) > 1e-6) {
      std::cout << "Slow AddVecVec is not equal to Neon AddVecVec." << std::endl;
      return 1;
    }
  }
  std::cout << "Slow AddVecVec is equal to Neon AddVecVec." << std::endl;
  return 0;
}
