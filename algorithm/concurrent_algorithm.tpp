template<typename GraphNode>
void
concurrent_algorithm(GraphNode* start_node)
{
    using GNiterator = typename GraphNode::iterator;

    std::stack<GraphNode*>                        stack_path_nodes;
    std::stack<GraphNode*>                        stack_explore_nodes;
    std::stack<std::pair<GNiterator, GNiterator>> stack_explore_pairs;

    // dfs init
    stack_path_nodes.emplace(start_node);
    stack_explore_nodes.emplace(start_node);
    stack_explore_pairs.emplace(start_node->get_random_neighbors_iterators());

    // iterative dfs
    while (!stack_explore_nodes.empty() && !stack_explore_pairs.empty())
    {
        // check if there's anything more to see from stack_explore_graph_nodes.top() node
        if (stack_explore_pairs.top().first == stack_explore_pairs.top().second)
    }

}
