#define _GNU_SOURCE // for sched_getaffinity

#include "assert.h"
#include "cpuid.h"
#include "glob.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/stat.h"
#include "unistd.h"

#include "omp.h"

#include "sched.h"
#include "hardware.h"

const uint64_t HARDWARE_HAS_SSE = 0x00;
const uint64_t HARDWARE_HAS_AVX2 = 0x01;
const uint64_t HARDWARE_HAS_AVX512 = 0x02;

uint64_t hardware_is_needlessly_disabled() {
  int a = 0, b = 0, c = 0, d = 0;
  while(a == 0) {
    __cpuid(0x01, a, b, c, d);
  }
  const int family = a >> 8 & (int)0x0F;
  const int model = (a >> 4 & (int)0x0F) | (a >> 12) & (int)0xF0;
  const int stepping = a & 0x0F;
  return (family == 6 && model == 151);
}

uint64_t hardware_instruction_set() {
  int a = 0, b = 0, c = 0, d = 0;
  while(b == 0) {
    __cpuid(0x07, a, b, c, d);
  }

  if(b & bit_AVX512F && b & bit_AVX512BW) {
    return HARDWARE_HAS_AVX512;
  } else if((b & bit_AVX2)) {
    return HARDWARE_HAS_AVX2;
  } else {
    return HARDWARE_HAS_SSE;
    exit(-1);
  }
}

uint64_t hardware_ram_speed() {
  glob_t dmiglob;
  uint16_t ram_speed;
  assert(!glob("/sys/firmware/dmi/entries/17-*/raw", 0, NULL, &dmiglob));
  for(size_t i = 0; i < dmiglob.gl_pathc; i++) {
    FILE* file = fopen(dmiglob.gl_pathv[i], "r");
    if(file == NULL) {
      fprintf(stderr, "could not stat dmi table; try running as root\n");
      exit(-1);
    }
    struct stat size;
    fseek(file, 0x15, SEEK_SET);
    while(!fread((void*) &ram_speed, sizeof(uint16_t), 1, file));
    fclose(file);
    if(ram_speed) {
      break;
    }
  }

  return (uint64_t) ram_speed;
}

uint64_t hardware_cpu_count() {
  cpu_set_t cpuset;
  size_t omp_thread_count;

  sched_getaffinity((pid_t)getpid(), sizeof(cpu_set_t), &cpuset);

  #pragma omp parallel
  {
    #pragma omp single
    omp_thread_count = omp_get_num_threads();
  }

  if(CPU_COUNT(&cpuset) < omp_thread_count){
    omp_set_num_threads(CPU_COUNT(&cpuset));
    return CPU_COUNT(&cpuset);
  } else {
    return omp_thread_count;
  }
}
