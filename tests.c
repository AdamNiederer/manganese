#include "assert.h"
#include "stdatomic.h"
#include "stdint.h"
#include "stdio.h"

#include "SIMDxorshift/include/simdxorshift128plus.h"

#include "hardware.h"
#include "tests.h"
#include "tests-512.h"
#include "tests-256.h"

const size_t tests_sz = 12;

static struct test_t avx2_tests[] = {
  {.name = "Basic Tests", .passes = 4, .iters = 6, .run = avx2_basic_tests},
  {.name = "March", .passes = 17, .iters = 2, .run = avx2_march},
  {.name = "Random Inversions", .passes = 4, .iters = 16, .run = avx2_random_inversions},
  {.name = "Moving Inversions <<64", .passes = 4, .iters = 64, .run = avx2_moving_inversions_left_64},
  {.name = "Moving Inversions 32>>", .passes = 4, .iters = 32, .run = avx2_moving_inversions_right_32},
  {.name = "Moving Inversions <<16", .passes = 4, .iters = 16, .run = avx2_moving_inversions_left_16},
  {.name = "Moving Inversions 8>>", .passes = 4, .iters = 8, .run = avx2_moving_inversions_right_8},
  {.name = "Moving Inversions <<4", .passes = 4, .iters = 4, .run = avx2_moving_inversions_left_4},
  {.name = "Moving Saturations 16>>", .passes = 8, .iters = 16, .run = avx2_moving_saturations_right_16},
  {.name = "Moving Saturations <<8", .passes = 8, .iters = 8, .run = avx2_moving_saturations_left_8},
  {.name = "Addressing", .passes = 2, .iters = 16, .run = avx2_addressing},
  {.name = "SGEMM", .passes = 1, .iters = 32, .run = avx2_sgemm},
};

static struct test_t avx512_tests[] = {
  {.name = "Basic Tests", .passes = 4, .iters = 6, .run = avx512_basic_tests},
  {.name = "March", .passes = 17, .iters = 2, .run = avx512_march},
  {.name = "Random Inversions", .passes = 4, .iters = 16, .run = avx512_random_inversions},
  {.name = "Moving Inversions <<64", .passes = 4, .iters = 64, .run = avx512_moving_inversions_left_64},
  {.name = "Moving Inversions 32>>", .passes = 4, .iters = 32, .run = avx512_moving_inversions_right_32},
  {.name = "Moving Inversions <<16", .passes = 4, .iters = 16, .run = avx512_moving_inversions_left_16},
  {.name = "Moving Inversions 8>>", .passes = 4, .iters = 8, .run = avx512_moving_inversions_right_8},
  {.name = "Moving Inversions <<4", .passes = 4, .iters = 4, .run = avx512_moving_inversions_left_4},
  {.name = "Moving Saturations 16>>", .passes = 8, .iters = 16, .run = avx512_moving_saturations_right_16},
  {.name = "Moving Saturations <<8", .passes = 8, .iters = 8, .run = avx512_moving_saturations_left_8},
  {.name = "Addressing", .passes = 4, .iters = 16, .run = avx512_addressing},
  {.name = "SGEMM", .passes = 1, .iters = 32, .run = avx512_sgemm},
};

struct test_t* tests_init(size_t cpus, _Atomic(uint64_t)* errors, const int isa) {
  if(isa == HARDWARE_HAS_AVX512) {
    avx512_tests_init(cpus, errors);
    return avx512_tests;
  } else if(isa == HARDWARE_HAS_AVX2) {
    avx2_tests_init(cpus, errors);
    return avx2_tests;
  }
}
