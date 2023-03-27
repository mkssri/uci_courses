// Sorter
// -----------------------------------------------------------------------------
//
// Helper class that provides a simple interface to execute the different
// bitonicsort implementations.
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#ifndef BITONICSORT_SORTER_H
#define BITONICSORT_SORTER_H

#include "bitonicsort.h"
#include "concurrency/policy.h"

namespace bitonicsort {

// =============================================================================
// Computes the logarithm base 2 of a power of 2.
inline static int log(int N) { return __builtin_ctz(N); }

class Sorter {
 public:
  explicit Sorter(int data_size, int segment_size, int num_threads)
      : data_size_(data_size),
        segment_size_(segment_size),
        num_threads_(num_threads),
        num_segments_(data_size / segment_size),
        bitonic_stages_((log(num_segments_) * (log(num_segments_) + 1)) / 2) {}

  explicit Sorter(int data_size, int segment_size)
      : Sorter(data_size, segment_size, /*num_threads=*/1) {}

  // ===========================================================================
  // Executes the algorithm given a concurrency policy.
  void Sort(std::vector<int>& data, concurrency::Policy policy) {
    switch (policy) {
      case concurrency::Policy::kSequential:
        Sequential(data, segment_size_);
        return;
      case concurrency::Policy::kBarrierBased:
        BarrierBased(data, segment_size_, num_threads_);
        return;
      case concurrency::Policy::kNonBlocking:
        NonBlocking(data, segment_size_, num_threads_);
        return;
      case concurrency::Policy::kOMPBased:
        OpenMPBased(data, segment_size_, num_threads_);
        return;
    }
  }

  // ===========================================================================
  // Returns the total number of data elements.
  int data_size() { return data_size_; }

  // ===========================================================================
  // Returns the number of elements in a data segment.
  int segment_size() { return segment_size_; }

  // ===========================================================================
  // Returns the number of concurrent threads.
  int num_threads() { return num_threads_; }

  // ===========================================================================
  // Returns the total number of data segments.
  int num_segments() { return num_segments_; }

  // ===========================================================================
  // Returns the number of stages in the bitonic network: O(log^2(N)).
  int bitonic_stages() { return bitonic_stages_; }

 private:
  // Number of total data elements.
  const int data_size_;

  // Number of elements in a data segment.
  const int segment_size_;

  // Number of concurrent threads.
  const int num_threads_;

  // Number of total segments.
  const int num_segments_;

  // Number of stages in the bitonic network.
  const int bitonic_stages_;
};

}  // namespace bitonicsort

#endif  // BITONICSORT_SORTER_H
