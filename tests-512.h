#include "unistd.h"

void avx512_tests_init(size_t cpus, _Atomic(uint64_t)* errors);

void avx512_basic_tests(void* const mem, const size_t size);
void avx512_march(void* const mem, const size_t size);
void avx512_random_inversions(void* const mem, const size_t size);
void avx512_moving_inversions_left_64(void* const mem, const size_t size);
void avx512_moving_inversions_right_32(void* const mem, const size_t size);
void avx512_moving_inversions_left_16(void* const mem, const size_t size);
void avx512_moving_inversions_right_8(void* const mem, const size_t size);
void avx512_moving_inversions_left_4(void* const mem, const size_t size);
void avx512_moving_saturations_right_16(void* const mem, const size_t size);
void avx512_moving_saturations_left_8(void* const mem, const size_t size);
void avx512_addressing(void* const mem, const size_t size);
