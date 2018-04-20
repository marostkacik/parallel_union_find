template<typename GraphNode>
void
simple_set_based_algorithm(GraphNode* start_node, const uint64_t thread_id)
{
    using GNiterator = typename GraphNode::iterator;

    std::stack<GraphNode*>                        stack_path_nodes;
    std::stack<GraphNode*>                        stack_explore_nodes_v;
    std::stack<std::pair<GNiterator, GNiterator>> stack_explore_nodes_iterators;

    // dfs init
    start_node->add_mask(thread_id);
    stack_path_nodes.emplace(start_node);
    stack_explore_nodes_v.emplace(start_node);
    stack_explore_nodes_iterators.emplace(start_node->get_random_neighbors_iterators());

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
            else if (!w->has_mask(thread_id))
            {
                w->add_mask(thread_id);
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
