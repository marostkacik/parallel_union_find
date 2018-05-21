#pragma once

#include <stack>

namespace parallel_union_find::algorithm
{
    template<typename GraphNode>
    void sequential_set_based_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "sequential_set_based_algorithm.tpp"
}
