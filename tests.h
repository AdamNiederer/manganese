struct test_t* tests_init(size_t cpus, _Atomic(uint64_t)* errors, const int isa);

struct test_t {
  const char* name;
  const size_t passes;
  const size_t iters;
  void (*run)(void* const restrict, const size_t);
};

extern const size_t tests_sz;
