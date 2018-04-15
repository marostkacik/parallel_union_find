#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::algorithm
{
// returns nullptr if and only if vp is part of set which has no active (not done) nodes
template<typename GraphNode>
GraphNode* get_vp_from(GraphNode* vp);

template<typename GraphNode>
void concurrent_algorithm(GraphNode* start_node, const uint64_t thread_id);

#include "concurrent_algorithm.tpp"
}
