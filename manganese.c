#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "assert.h"
#include "time.h"
#include "stdbool.h"

#include "sys/sysinfo.h"
#include "sys/mman.h"

#include "tests.h"
#include "hardware.h"

static _Atomic(uint64_t) ERRORS = 0;

int main(int argc, char** argv) {
  if(argc < 2) {
    fprintf(stderr, "usage: manganese [0%-99%]\n");
    return 1;
  }

  const double fraction = atof(argv[1]) / 100;

  const uint64_t cpu_count = hardware_cpu_count();
  const uint64_t ram_speed = hardware_ram_speed(true);
  const uint64_t isa = hardware_instruction_set();

  if(isa == HARDWARE_HAS_SSE) {
    fprintf(stderr, "AVX2 or AVX-512 not available, aborting\n");
    exit(-1);
  }

  struct sysinfo sys;
  assert(!sysinfo(&sys));
  const uint64_t total_alloc = (sys.totalram * fraction) - ((long)(sys.totalram * fraction) % (cpu_count * getpagesize()));

  const size_t backoff = 256 * 1024 * 1024;
  void* mem = NULL;
  size_t size;
  for(size_t i = 0; i <= (total_alloc / backoff); i++) {
    mem = aligned_alloc(cpu_count * getpagesize(), total_alloc - i * backoff);
    size = total_alloc - i * backoff;
    if(!mlock(mem, total_alloc - i * backoff)) {
      fprintf(stderr, "Threads           : %lu\n", cpu_count);
      if(ram_speed) {
        fprintf(stderr, "Memory Speed      : %luMT/s (%lu MB/s per channel)\n" , ram_speed, 8 * ram_speed);
      }
      fprintf(stderr, "Locked Memory     : %ldMB of %ldMB (%.0f%%)\n", size / (1024 * 1024), sys.totalram / (1024 * 1024), 100.0 * size / sys.totalram);
      fprintf(stderr, "Chunk Alignment   : %luK\n" , cpu_count * getpagesize() / 1024);
      if(isa == HARDWARE_HAS_AVX512) {
        fprintf(stderr, "Instruction Set   : AVX-512\n");
      } else if(hardware_is_needlessly_disabled()) {
        fprintf(stderr, "Instruction Set   : AVX2 (lol)\n");
      } else {
        fprintf(stderr, "Instruction Set   : AVX2\n");
      }

      fprintf(stderr, "\n");

      break;
    } else {
      free(mem);
      mem = NULL;
    }
    if((total_alloc - i * backoff) <= backoff) {
      fprintf(stderr, "can't lock any memory; try increasing memlock ulimit or running as root\n");
      exit(-1);
    }
  }

  const struct test_t* tests = tests_init(cpu_count, &ERRORS, isa);

  while(1) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for(size_t i = 0; i < tests_sz; i++) {
      printf("Running: %s\n", tests[i].name);
      tests[i].run(mem, size);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    size_t total_passes = 0;
    for(size_t i = 0; i < tests_sz; i++) {
      total_passes += tests[i].passes * tests[i].iters;
    }

    const double total_time = (double)(end.tv_sec - start.tv_sec) + (0.000000001 * (end.tv_nsec - start.tv_nsec));
    const double bandwidth = (total_passes * ((double)size / (1024 * 1024))) / total_time;
    fprintf(stderr, "Tests completed in %.2f sec [%.0fMB/s]\n", total_time, bandwidth);
    if(ERRORS) {
      fprintf(stderr, "\e[1;91m" "%ld errors detected" "\e[0m" "\n", ERRORS);
    }
  }
}
