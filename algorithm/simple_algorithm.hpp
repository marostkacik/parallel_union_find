#pragma once

#include <stack>
#include <unordered_map>

namespace parallel_union_find::algorithm
{
    template<typename GraphNode>
    void simple_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "simple_algorithm.tpp"
}
