// Bitonicsort Microbenchmarks
// -----------------------------------------------------------------------------
//
// The following microbenchmarks measure the real time to sort a `std::vector`
// using the different implementations of bitonicsort. The experiments show the
// gains in performance due to multithreaded parallelization and the
// non-blocking properties of the algorithm.
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#include <benchmark/benchmark.h>

#include <cassert>
#include <string>

#include "sorter.h"

// =============================================================================
// Benchmark arguments.
static void CustomArgs(benchmark::internal::Benchmark* b) {
  // Input data.
  const int data_size = 1 << 19;          // 2048 kB.
  const int segment_size_begin = 1 << 8;  // 1 kB.
  const int segment_size_end = 1 << 18;   // 1024 kB.

  // Concurrency policies.
  //   + 0: Sequential.
  //   + 1: Barrier-based.
  //   + 2: Non-blocking.
  //   + 3: OpenMP.
  const int policy_begin = 0;
  const int policy_end = 3;

  // The maximum number of threads is given by hardware.
  const int num_threads_begin = 1;
  const int num_threads_end = std::thread::hardware_concurrency();

  for (int policy = policy_begin; policy <= policy_end; ++policy) {
    if (policy == 0) {
      // Sequential.
      for (int segment_size = segment_size_begin;
           segment_size <= segment_size_end; segment_size <<= 2)
        b->Args({data_size, segment_size, policy, /*num_threads=*/1});
    } else {
      // Multithreaded.
      for (int num_threads = num_threads_begin; num_threads <= num_threads_end;
           num_threads <<= 1)
        b->Args({data_size, segment_size_begin, policy, num_threads});
    }
  }
}

// =============================================================================
// Checks if vector is correctly sorted in increasing (non-decreasing).
inline static bool IsSortedIncreasing(std::vector<int>& data) {
  for (size_t i = 1; i < data.size(); ++i)
    if (data[i - 1] > data[i]) return false;
  return true;
}

// =============================================================================
// Returns a random vector of a given size.
inline static std::vector<int> CreateRandomVector(int data_size) {
  std::vector<int> data;
  data.reserve(data_size);
  for (int i = 0; i < data_size; ++i) data.push_back(i);
  std::random_shuffle(data.begin(), data.end());
  return data;
}

inline static void SetLabel(benchmark::State& state,
                            bitonicsort::Sorter* sorter,
                            concurrency::Policy policy) {
  const int data_bytes = sorter->data_size() * sizeof(int);
  const int segment_bytes = sorter->segment_size() * sizeof(int);
  std::string label = std::to_string(data_bytes / 1024) + " kB data / " +
                      std::to_string(segment_bytes / 1024) + " kB segment / " +
                      std::to_string(sorter->num_segments()) + " segments / " +
                      std::to_string(sorter->num_threads()) + " threads / " +
                      std::to_string(sorter->bitonic_stages()) + " stages / ";
  if (policy == concurrency::Policy::kSequential)
    label += "sequential";
  else if (policy == concurrency::Policy::kBarrierBased)
    label += "barrier-based";
  else if (policy == concurrency::Policy::kNonBlocking)
    label += "non-blocking";
  else if (policy == concurrency::Policy::kOMPBased)
    label += "openmp";
  state.SetBytesProcessed(state.iterations() * data_bytes);
  state.SetLabel(label);
}

// =============================================================================
// Benchmark: Bitonicsort.
static void BM_Bitonicsort(benchmark::State& state) {
  // Setup.
  const int data_size = state.range(0);
  const int segment_length = state.range(1);
  const concurrency::Policy concurrency_policy =
      static_cast<concurrency::Policy>(state.range(2));
  const int num_threads = state.range(3);
  std::vector<int> data = CreateRandomVector(data_size);
  bitonicsort::Sorter* sorter =
      new bitonicsort::Sorter(data_size, segment_length, num_threads);

  // Benchmark.
  for (auto _ : state) {
    sorter->Sort(data, concurrency_policy);

    // Prepare next iteration.
    state.PauseTiming();
    assert(IsSortedIncreasing(data) && "Data is not sorted");
    std::random_shuffle(data.begin(), data.end());
    state.ResumeTiming();
  }

  // Teardown.
  SetLabel(state, sorter, concurrency_policy);
  delete sorter;
}

// =============================================================================
// Register benchmark.
BENCHMARK(BM_Bitonicsort)
    ->Apply(CustomArgs)
    ->Unit(benchmark::kMillisecond)
    ->UseRealTime();
