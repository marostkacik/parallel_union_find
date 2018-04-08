#include <bits/stdc++.h>

namespace parallel_union_find::algorithm
{
// no communication between threads, each thread independently decides where to go next.
// threads communicate only when some thread is completely done
template<typename GraphNode>
void simple_algorithm(GraphNode* start_node);

#include "simple_algorithm.tpp"
}
