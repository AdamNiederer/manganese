extern const uint64_t HARDWARE_HAS_SSE;
extern const uint64_t HARDWARE_HAS_AVX2;
extern const uint64_t HARDWARE_HAS_AVX512;

uint64_t hardware_is_needlessly_disabled();
uint64_t hardware_instruction_set();
uint64_t hardware_ram_speed(bool configured);
uint64_t hardware_cpu_count();
