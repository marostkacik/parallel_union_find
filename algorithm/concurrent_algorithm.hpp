#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::algorithm
{
template<typename GraphNode>
void concurrent_algorithm(GraphNode* start_node);

#include "concurrent_algorithm.tpp"
}
