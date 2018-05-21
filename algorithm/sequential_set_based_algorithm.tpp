template<typename GraphNode>
void
sequential_set_based_algorithm(GraphNode* start_node, const uint64_t thread_id)
{
    using GNiterator = typename GraphNode::iterator;

    std::stack<GraphNode*>                        stack_path_nodes;
    std::stack<GraphNode*>                        stack_explore_nodes_v;
    std::stack<std::pair<GNiterator, GNiterator>> stack_explore_nodes_iterators;
    std::unordered_set<GraphNode*>                seen_graph_nodes;

    // dfs init
    stack_path_nodes.emplace(start_node);
    stack_explore_nodes_v.emplace(start_node);
    stack_explore_nodes_iterators.emplace(start_node->get_random_neighbors_iterators());
    seen_graph_nodes.insert(start_node);

    while (!stack_explore_nodes_v.empty())
        if (stack_explore_nodes_iterators.top().first == stack_explore_nodes_iterators.top().second)
        {
            if (stack_explore_nodes_v.top() == stack_path_nodes.top())
                stack_explore_nodes_v.top()->mark_as_dead(),
                stack_path_nodes.pop();

            stack_explore_nodes_v.pop();
            stack_explore_nodes_iterators.pop();
        }
        else
        {
            GraphNode* w = *stack_explore_nodes_iterators.top().first;
            ++stack_explore_nodes_iterators.top().first;

            if (w->is_dead());
            else if (seen_graph_nodes.find(w) == seen_graph_nodes.end())
            {
                seen_graph_nodes.insert(w);
                stack_path_nodes.emplace(w);
                stack_explore_nodes_v.emplace(w);
                stack_explore_nodes_iterators.emplace(w->get_random_neighbors_iterators());
            }
            else
                while (!stack_explore_nodes_v.top()->same_set(w))
                {
                    GraphNode* tmp = stack_path_nodes.top();
                    stack_path_nodes.pop();

                    while (!tmp->union_set(stack_path_nodes.top()));
                }
        }
}
