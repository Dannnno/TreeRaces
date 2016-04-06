#ifdef CPU_OCTREE
#include "../structures/octree.h"

template <typename InputIterator, class PointExtractor, 
          size_t max_per_node, size_t max_depth>
using Octree_Implementation = Octree<InputIterator, PointExtractor, max_per_node, max_depth>;

#endif

#ifndef NUM_TRIALS
#define NUM_TRIALS 10
#endif

#ifndef TOLERANCE
#define TOLERANCE 0.00001
#endif

void benchmark_small_even_dispersion();
void benchmark_large_even_dispersion();
void benchmark_small_uneven_dispersion();
void benchmark_large_mostlyeven_dispersion();
