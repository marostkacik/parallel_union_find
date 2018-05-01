#pragma once

#include <stack>

namespace parallel_union_find::algorithm
{
    template<typename GraphNode>
    void simple_set_based_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "simple_set_based_algorithm.tpp"
}
