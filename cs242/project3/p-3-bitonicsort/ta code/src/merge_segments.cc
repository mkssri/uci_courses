// Implements `merge_segments.h`
//
// -----------------------------------------------------------------------------
// Author: Arturo Garza-Rodriguez

#include "merge_segments.h"

namespace bitonicsort {

// =============================================================================
// Scatter data back into original arrays.
static void Scatter(int* buffer, int* segment1, int* segment2, int size) {
  const int bytes = size * sizeof(int);
  std::memcpy(/*dst=*/segment1, /*src=*/buffer, /*cnt=*/bytes);
  std::memcpy(/*dst=*/segment2, /*src=*/buffer + size, /*cnt=*/bytes);
}

// =============================================================================
// Merge in inreasing order from two increasing sorted segments.
static void MergeUpFromUpUp(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = 0, j = 0, k = 0;
  while (i < size && j < size) {
    if (segment1[i] < segment2[j])
      buffer[k++] = segment1[i++];
    else
      buffer[k++] = segment2[j++];
  }
  while (i < size) buffer[k++] = segment1[i++];
  while (j < size) buffer[k++] = segment2[j++];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in inreasing order from increasing & decreasing sorted segments.
static void MergeUpFromUpDn(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = 0, j = size - 1, k = 0;
  while (i < size && j >= 0) {
    if (segment1[i] < segment2[j])
      buffer[k++] = segment1[i++];
    else
      buffer[k++] = segment2[j--];
  }
  while (i < size) buffer[k++] = segment1[i++];
  while (j >= 0) buffer[k++] = segment2[j--];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in inreasing order from decreasing & increasing sorted segments.
static void MergeUpFromDnUp(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = size - 1, j = 0, k = 0;
  while (i >= 0 && j < size) {
    if (segment1[i] < segment2[j])
      buffer[k++] = segment1[i--];
    else
      buffer[k++] = segment2[j++];
  }
  while (i >= 0) buffer[k++] = segment1[i--];
  while (j < size) buffer[k++] = segment2[j++];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in inreasing order from decreasing & decreasing sorted segments.
static void MergeUpFromDnDn(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = size - 1, j = size - 1, k = 0;
  while (i >= 0 && j >= 0) {
    if (segment1[i] < segment2[j])
      buffer[k++] = segment1[i--];
    else
      buffer[k++] = segment2[j--];
  }
  while (i >= 0) buffer[k++] = segment1[i--];
  while (j >= 0) buffer[k++] = segment2[j--];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in decreasing order from increasing & increasing sorted segments.
static void MergeDnFromUpUp(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = size - 1, j = size - 1, k = 0;
  while (i >= 0 && j >= 0) {
    if (segment1[i] > segment2[j])
      buffer[k++] = segment1[i--];
    else
      buffer[k++] = segment2[j--];
  }
  while (i >= 0) buffer[k++] = segment1[i--];
  while (j >= 0) buffer[k++] = segment2[j--];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in decreasing order from increasing & decreasing sorted segments.
static void MergeDnFromUpDn(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = size - 1, j = 0, k = 0;
  while (i >= 0 && j < size) {
    if (segment1[i] > segment2[j])
      buffer[k++] = segment1[i--];
    else
      buffer[k++] = segment2[j++];
  }
  while (i >= 0) buffer[k++] = segment1[i--];
  while (j < size) buffer[k++] = segment2[j++];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in decreasing order from decreasing & increasing sorted segments.
static void MergeDnFromDnUp(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = 0, j = size - 1, k = 0;
  while (i < size && j >= 0) {
    if (segment1[i] > segment2[j])
      buffer[k++] = segment1[i++];
    else
      buffer[k++] = segment2[j--];
  }
  while (i < size) buffer[k++] = segment1[i++];
  while (j >= 0) buffer[k++] = segment2[j--];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in decreasing order from two decreasing sorted segments.
static void MergeDnFromDnDn(int* segment1, int* segment2, int size,
                            int* buffer) {
  // Merge into buffer.
  int i = 0, j = 0, k = 0;
  while (i < size && j < size) {
    if (segment1[i] > segment2[j])
      buffer[k++] = segment1[i++];
    else
      buffer[k++] = segment2[j++];
  }
  while (i < size) buffer[k++] = segment1[i++];
  while (j < size) buffer[k++] = segment2[j++];

  // Scatter back into original arrays.
  Scatter(buffer, segment1, segment2, size);
}

// =============================================================================
// Merge in increasing (non-decreasing) order.
void MergeUp(int* segment1, int* segment2, int size, int* buffer) {
  const int l = 0, r = size - 1;
  if (segment1[l] < segment1[r] && segment2[l] < segment2[r])
    MergeUpFromUpUp(segment1, segment2, size, buffer);
  else if (segment1[l] < segment1[r] && segment2[l] > segment2[r])
    MergeUpFromUpDn(segment1, segment2, size, buffer);
  else if (segment1[l] > segment1[r] && segment2[l] < segment2[r])
    MergeUpFromDnUp(segment1, segment2, size, buffer);
  else
    MergeUpFromDnDn(segment1, segment2, size, buffer);
}

// =============================================================================
// Merge in decreasing (non-increasing) order.
void MergeDn(int* segment1, int* segment2, int size, int* buffer) {
  const int l = 0, r = size - 1;
  if (segment1[l] < segment1[r] && segment2[l] < segment2[r])
    MergeDnFromUpUp(segment1, segment2, size, buffer);
  else if (segment1[l] < segment1[r] && segment2[l] > segment2[r])
    MergeDnFromUpDn(segment1, segment2, size, buffer);
  else if (segment1[l] > segment1[r] && segment2[l] < segment2[r])
    MergeDnFromDnUp(segment1, segment2, size, buffer);
  else
    MergeDnFromDnDn(segment1, segment2, size, buffer);
}

}  // namespace bitonicsort
