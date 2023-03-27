// Implements `bitonicsort.h`
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#include "bitonicsort.h"

namespace bitonicsort {

// =============================================================================
void Sequential(std::vector<int>& data, int segment_size) {
  // Setup.
  const int data_size = data.size();
  const int num_segments = data_size / segment_size;
  std::vector<int> buffer(2 * segment_size);

  // Sort each indiviual segment.
  for (int i = 0; i < data_size; i += segment_size)
    std::sort(data.begin() + i, data.begin() + i + segment_size);

  // Bitonic merging network.
  for (int k = 2; k <= num_segments; k <<= 1) {
    for (int j = k >> 1; j > 0; j >>= 1) {
      for (int i = 0; i < num_segments; ++i) {
        const int ij = i ^ j;
        if (i < ij) {
          if ((i & k) == 0)
            MergeUp(/*segment1=*/&data[i * segment_size],
                    /*segment2=*/&data[ij * segment_size],
                    /*segment_size=*/segment_size,
                    /*buffer=*/buffer.data());
          else
            MergeDn(/*segment1=*/&data[i * segment_size],
                    /*segment2=*/&data[ij * segment_size],
                    /*segment_size=*/segment_size,
                    /*buffer=*/buffer.data());
        }
      }
    }
  }
}

// =============================================================================
static void BarrierBasedThread(std::vector<int>& data, int segment_size,
                               int num_threads, int thread_id, void* arg) {
  const int data_size = data.size();
  const int num_segments = data_size / segment_size;
  const int num_segments_per_thread = num_segments / num_threads;
  const int low_segment = thread_id * num_segments_per_thread;
  const int high_segment = low_segment + num_segments_per_thread;
  const int low_index = low_segment * segment_size;
  const int high_index = high_segment * segment_size;
  std::vector<int> buffer(2 * segment_size);
  concurrency::Barrier* barrier = static_cast<concurrency::Barrier*>(arg);

  // Sort each segment.
  for (int i = low_index; i < high_index; i += segment_size)
    std::sort(data.begin() + i, data.begin() + i + segment_size);

  // Barrier synchronization.
  barrier->Wait(num_threads);

  // Bitonic merging network.
  for (int k = 2; k <= num_segments; k <<= 1) {
    for (int j = k >> 1; j > 0; j >>= 1) {
      for (int i = low_segment; i < high_segment; ++i) {
        const int ij = i ^ j;
        if (i < ij) {
          if ((i & k) == 0) {
            MergeUp(/*segment1=*/&data[i * segment_size],
                    /*segment2=*/&data[ij * segment_size],
                    /*segment_size=*/segment_size,
                    /*buffer=*/buffer.data());
          } else {
            MergeDn(/*segment1=*/&data[i * segment_size],
                    /*segment2=*/&data[ij * segment_size],
                    /*segment_size=*/segment_size,
                    /*buffer=*/buffer.data());
          }
        }
      }

      // Barrier synchronization.
      barrier->Wait(num_threads);
    }
  }
}

// =============================================================================
static void NonBlockingThread(std::vector<int>& data, int segment_size,
                              int num_threads, int thread_id, void* arg) {
  const int data_size = data.size();
  const int num_segments = data_size / segment_size;
  const int num_segments_per_thread = num_segments / num_threads;
  const int low_segment = thread_id * num_segments_per_thread;
  const int high_segment = low_segment + num_segments_per_thread;
  const int low_index = low_segment * segment_size;
  const int high_index = high_segment * segment_size;
  std::vector<int> buffer(2 * segment_size);
  concurrency::AtomicInt** stages = static_cast<concurrency::AtomicInt**>(arg);

  // Initial stage.
  int my_stage = 0;

  // Sort each segment.
  for (int i = low_index; i < high_index; i += segment_size) {
    std::sort(data.begin() + i, data.begin() + i + segment_size);

    // Mark segment "ready" for next stage.
    const int segment_id = i / segment_size;
    stages[segment_id]->count_.fetch_add(1, std::memory_order_release);
  }

  // Mark this thread "ready" for next stage.
  ++my_stage;

  // Bitonic merging network.
  for (int k = 2; k <= num_segments; k <<= 1) {
    for (int j = k >> 1; j > 0; j >>= 1) {
      for (int i = low_segment; i < high_segment; ++i) {
        const int ij = i ^ j;
        if (i < ij) {
          const int segment1_index = i * segment_size;
          const int segment2_index = ij * segment_size;
          const int segment1_id = segment1_index / segment_size;
          const int segment2_id = segment2_index / segment_size;

          // Wait until the specific segments are on my same stage.
          while (my_stage !=
                 (stages[segment1_id]->count_.load(std::memory_order_relaxed)))
            continue;
          while (my_stage !=
                 (stages[segment2_id]->count_.load(std::memory_order_acquire)))
            continue;

          if ((i & k) == 0)
            MergeUp(/*segment1=*/&data[segment1_index],
                    /*segment2=*/&data[segment2_index],
                    /*segment_size=*/segment_size,
                    /*buffer=*/buffer.data());
          else
            MergeDn(/*segment1=*/&data[segment1_index],
                    /*segment2=*/&data[segment2_index],
                    /*segment_size=*/segment_size,
                    /*buffer=*/buffer.data());

          // Mark segments "ready" for next stage.
          stages[segment1_id]->count_.fetch_add(1, std::memory_order_relaxed);
          stages[segment2_id]->count_.fetch_add(1, std::memory_order_release);
        }
      }

      // Mark this thread "ready" for next stage.
      ++my_stage;
    }
  }
}

// =============================================================================
static void LaunchThreads(
    std::vector<int>& data, int segment_size, int num_threads, void* arg,
    std::function<void(std::vector<int>&, int, int, int, void*)> model) {
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Launch threads.
  // Main thread also performs work as thread 0, so loop starts in 1.
  for (int thread_id = 1; thread_id < num_threads; ++thread_id)
    threads.emplace_back([&, segment_size, num_threads, thread_id, arg] {
      model(data, segment_size, num_threads, thread_id, arg);
    });
  model(data, segment_size, num_threads, /*thread_id=*/0, arg);

  // Join threads.
  // So main thread can acquire the last published changes of the other threads.
  for (auto& thread : threads) thread.join();
}

// =============================================================================
void BarrierBased(std::vector<int>& data, int segment_size, int num_threads) {
  concurrency::Barrier* barrier = new concurrency::Barrier();

  LaunchThreads(data, segment_size, num_threads,
                /*arg=*/static_cast<void*>(barrier),
                /*model=*/&BarrierBasedThread);

  delete barrier;
}

// =============================================================================
void NonBlocking(std::vector<int>& data, int segment_size, int num_threads) {
  const int num_segments = data.size() / segment_size;
  std::vector<concurrency::AtomicInt*> stages;
  stages.reserve(num_threads);
  for (int i = 0; i < num_segments; ++i)
    stages.emplace_back(new concurrency::AtomicInt());

  LaunchThreads(data, segment_size, num_threads,
                /*arg=*/static_cast<void*>(stages.data()),
                /*model=*/&NonBlockingThread);

  for (auto segment_stage : stages) delete segment_stage;
}

// =============================================================================
void OpenMPBased(std::vector<int>& data, int segment_size, int num_threads) {
  const int data_size = data.size();
  const int num_segments = data_size / segment_size;
  omp_set_dynamic(0);
  omp_set_num_threads(num_threads);

#pragma omp parallel shared(data)
  {
    std::vector<int> buffer(2 * segment_size);

    // Sort each indiviual segment.
#pragma omp for
    for (int i = 0; i < data_size; i += segment_size)
      std::sort(data.begin() + i, data.begin() + i + segment_size);

    // Bitonic merging network.
    for (int k = 2; k <= num_segments; k <<= 1) {
      for (int j = k >> 1; j > 0; j >>= 1) {
#pragma omp for
        for (int i = 0; i < num_segments; ++i) {
          const int ij = i ^ j;
          if (i < ij) {
            if ((i & k) == 0)
              MergeUp(/*segment1=*/&data[i * segment_size],
                      /*segment2=*/&data[ij * segment_size],
                      /*segment_size=*/segment_size,
                      /*buffer=*/buffer.data());
            else
              MergeDn(/*segment1=*/&data[i * segment_size],
                      /*segment2=*/&data[ij * segment_size],
                      /*segment_size=*/segment_size,
                      /*buffer=*/buffer.data());
          }
        }
      }
    }
  }
}

}  // namespace bitonicsort
