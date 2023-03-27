# B I T O N I C S O R T

___As a vehicle to study different concurrency models for barrier synchronization in shared-memory computer architectures___

In a wide range of parallel algorithms some sort of synchronization is necessary. In this case, due to data dependencies between stages of the bitonic network, a barrier synchronization primitive is the one in charge to protect the concurrent reads and writes that take place across those stages. All execution threads wait at the barrier until all of them reach that point. A major downside is the _blocking_ nature of a barrier primitive. One single preempted thread will stop execution of all the other threads as they keep waiting in the barrier without doing any useful work. A solution to this _unnecessary waiting_ problem is proposed by exploiting the memory access pattern of the algorithm.

This bitonicsort implementation is presented as a generalization of the original algorithm. It is based on _merge_ operations on data segments instead of _compare-exchange_ operations on individual data elements.

## Requirements
- c++11
- gcc
- [google benchmark](https://github.com/google/benchmark)

After cloning this repository, build and execute the bitonicsort microbenchmarks by simply:

```bash
$ cd test/
$ make -j
```

## Makefile Notes
- The google benchmark library needs to be globally installed in order to build the repository
- The project is built into a _mybuilds/_ folder in your home path: _/home/$(USER)/mybuilds/bitonicsort_

## Implementations
- __Sequential__ implementation without any form of multithreading or concurrency model. Single thread performing all computational merges between segments of all the bitonic network.
- __Barrier-based__ multithreaded implementation using pthreads. Concurrency is handled via an explicit barrier synchronization primitive, where all threads _wait_ until all other execution threads reach the same point.
- __Non-blocking__ multithreaded implementation using pthreads. Concurrency is _data-driven_. The bitonicsort algorithm exposes a regular memory access pattern. This is, it is possible to infer which data segments will be merged at which stage of the bitonic network. With this knowledge, it is possible to create a "self-synchronizable" version of the algorithm with _lock-free_ thread progression guarantees.
- __OpenMP-based__ multithreaded implementation. Concurrency is delegated to the OpenMP runtime and its implicit barrier synchronization primitive.

__Note:__ Current version only sorts `std::vector` of `int` data type in increasing (non-decreasing) order.

__Author:__ Arturo Garza-Rodriguez
