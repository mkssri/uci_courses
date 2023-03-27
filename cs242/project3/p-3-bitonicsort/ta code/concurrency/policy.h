// Concurrency Policies
// -----------------------------------------------------------------------------
//
// Concurrency policies for the segmented bitonicsort algorithm. More details
// can be found in `bitonicsort.h`.
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#ifndef BITONICSORT_CONCURRENCY_POLICY_H
#define BITONICSORT_CONCURRENCY_POLICY_H

namespace concurrency {

enum class Policy {
  kSequential = 0,    // Sequential behavior, no concurrency.
  kBarrierBased = 1,  // Uses a barrier synchronization primitive.
  kNonBlocking = 2,   // Exploits the memory access pattern of the algorithm.
  kOMPBased = 3,      // Uses the OpenMP runtime and implicit barrier.
};

}  // namespace concurrency

#endif  // BITONICSORT_CONCURRENCY_POLICY_H
