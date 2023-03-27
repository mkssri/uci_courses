// Atomic Int
// -----------------------------------------------------------------------------
//
// A cache line aligned `std::atomic` integer wrapper.
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#ifndef BITONICSORT_CONCURRENCY_ATOMIC_INT_H
#define BITONICSORT_CONCURRENCY_ATOMIC_INT_H

#include <atomic>

#include "util/globals.h"

namespace concurrency {

struct AtomicInt {
  alignas(kCacheLineSize) std::atomic<int> count_;
  char padding_[kCacheLineSize - sizeof(std::atomic<int>)];

  explicit AtomicInt() : count_(0){};
};

}  // namespace concurrency

#endif  // BITONICSORT_CONCURRENCY_ATOMIC_INT_H
