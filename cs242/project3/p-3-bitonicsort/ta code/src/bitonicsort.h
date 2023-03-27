// Segmented Bitonicsort
// -----------------------------------------------------------------------------
//
// This is an implementation of the generalized version of the so-called
// bitonicsort algorithm for shared-memory computer architectures. It is based
// on `merge` operations on data segments, instead of `compare-exchange`
// operations on individual data elements. Initially, all segments are
// individually sorted. After that, each sorted segmented is processed by the
// bitonic-merging network. In the end, all the input data is globally sorted.
//
// There are different versions of the algorithm.
//
//   + A sequential (not multithreaded) implementation, where a single execution
//     thread will perform all the computational stages of the bitonic network.
//
//   + A barrier-based multithreaded implementation. Due to data dependencies
//     among the different stages of the algorithm, a barrier synchronization
//     primitive is used to protect the concurrent reads and writes between the
//     data segments being merged at a given stage.
//
//   + A non-blocking multithreaded implementation. Due to the regular memory
//     access pattern that is exposed by the algorithm, it is possible to bypass
//     the use of a synchronization primitive. Therefore, creating what I call a
//     "self-synchronizable" algorithm by exploiting the memory access pattern
//     with lock-free progression guarantees. The idea is to keep track of which
//     segment is being worked on by which thread during which stage (Spatial &
//     Temporal Code/Data Convergence).
//
//   + An OpenMP-based implementation. The concurrency model is delegated to the
//     OpenMP runtime and its barrier synchronization primitive.
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#ifndef BITONICSORT_BITONICSORT_H
#define BITONICSORT_BITONICSORT_H

#include <omp.h>

#include <algorithm>
#include <functional>
#include <thread>
#include <vector>

#include "concurrency/atomic_int.h"
#include "concurrency/barrier.h"
#include "merge_segments.h"
#include "util/globals.h"

namespace bitonicsort {

void Sequential(std::vector<int>& data, int segment_size);
void BarrierBased(std::vector<int>& data, int segment_size, int num_threads);
void NonBlocking(std::vector<int>& data, int segment_size, int num_threads);
void OpenMPBased(std::vector<int>& data, int segment_size, int num_threads);

}  // namespace bitonicsort

#endif  // BITONICSORT_BITONICSORT_H
