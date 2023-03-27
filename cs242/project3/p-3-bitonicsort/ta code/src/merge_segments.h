// Merge Segments
// -----------------------------------------------------------------------------
//
// Helper functions to merge segments of data of the same size. The merging is
// performed in place. This is, the result is stored in the original data
// segments. However, for practical reasons, a helper buffer is used in order to
// perform the merge in linear-time. The source file contains more details on
// each function. It includes all possible merging combinations, for example
// merging in increasing order when one input segment is sorted in increasing
// order and the other one is sorted in decreasing order (and so on and so
// forth).
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#ifndef BITONICSORT_MERGE_SEGMENTS_H
#define BITONICSORT_MERGE_SEGMENTS_H

#include <cstring>

namespace bitonicsort {

// Merges two segments of the same size in increasing (non-decreasing) order.
void MergeUp(int* segment1, int* segment2, int size, int* buffer);

// Merges two segments of the same size in decreasing (non-increasing) order.
void MergeDn(int* segment1, int* segment2, int size, int* buffer);

}  // namespace bitonicsort

#endif  // BITONICSORT_MERGE_SEGMENTS_H
