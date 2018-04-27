#pragma once

#include <stack>

namespace parallel_union_find::algorithm
{
// no communication between threads, each thread independently decides where to go next.
// threads communicate only when some thread is completely done
template<typename GraphNode>
void simple_set_based_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "simple_set_based_algorithm.tpp"
}
