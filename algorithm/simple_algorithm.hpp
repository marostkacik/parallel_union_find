#include <bits/stdc++.h>

namespace parallel_union_find::algorithm
{
template<typename GraphNode, typename Node>
class Solver
{
public:
    void                                 run(GraphNode* start_node);
    std::vector<std::vector<GraphNode*>> get_components();

private:

private: // TODO, translate it to code
    // GraphNode must have these methods
    // std::pair<GraphNode::iterator, GraphNode::iterator> get_random_neighbors_iterators();
    // Node* get_node()
};

#include "simple_algorithm.tpp"
}
