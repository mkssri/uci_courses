// Barrier Synchronization Primitive
// -----------------------------------------------------------------------------
//
// This is a simple implementation of a counter-based spinning barrier that
// guarantees that no thread can continue execution of the program until all
// other threads reach the barrier. The atomic variables are aligned to the size
// of a cacheline to avoid false-sharing among multiple CPU cores and a more
// relaxed memory model is used in order to help improve performance.
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#ifndef BITONICSORT_CONCURRENCY_BARRIER_H
#define BITONICSORT_CONCURRENCY_BARRIER_H

#include <atomic>

#include "util/globals.h"

namespace concurrency {

class Barrier {
 public:
  // ===========================================================================
  void Wait(int num_threads) {
    unsigned int current_step = step_.load(std::memory_order_acquire);
    if (spinning_threads_.fetch_add(1) < num_threads - 1) {
      // Spin until last thread arrives.
      while (step_.load(std::memory_order_acquire) == current_step) continue;
    } else {
      // Last thread enters the barrier.
      // Reset number of spinning threads and increase the step.
      spinning_threads_.store(0, std::memory_order_relaxed);
      step_.fetch_add(1, std::memory_order_release);
    }
  }

 private:
  // Current number of threads spinning in the barrier.
  alignas(kCacheLineSize) std::atomic<int> spinning_threads_{0};

  // Current number of times the barrier has been used.
  // The barrier is reusable since unsigned data types wrap around the overflow.
  alignas(kCacheLineSize) std::atomic<unsigned int> step_{0};
};

}  // namespace concurrency

#endif  // BITONICSORT_CONCURRENCY_BARRIER_H
