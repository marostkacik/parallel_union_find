#pragma once

#include <stack>

namespace parallel_union_find::algorithm
{
    namespace internal_
    {
        template<typename GraphNode>
        GraphNode* get_vp_from(GraphNode* vp);
    }

    template<typename GraphNode>
    void concurrent_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "concurrent_algorithm.tpp"
}
