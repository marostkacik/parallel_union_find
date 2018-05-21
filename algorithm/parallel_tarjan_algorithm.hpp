#pragma once

#include <stack>
#include <unordered_map>

namespace parallel_union_find::algorithm
{
    template<typename GraphNode>
    void parallel_tarjan_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "parallel_tarjan_algorithm.tpp"
}
