#pragma once

#include <stack>
#include <cassert>

namespace parallel_union_find::algorithm
{
    namespace internal_
    {
        template<typename GraphNode>
        GraphNode* get_vp_from(GraphNode* vp);
    }

    template<typename GraphNode>
    void multi_core_on_the_fly_scc_decomposition_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "multi_core_on_the_fly_scc_decomposition_algorithm.tpp"
}
