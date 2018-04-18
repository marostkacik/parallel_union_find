template<typename GraphNode>
void
simple_algorithm(GraphNode* start_node, const uint64_t thread_id)
{
    using GNiterator = typename GraphNode::iterator;

    size_t timestamp = 0;
    std::unordered_map<GraphNode*, size_t>        seen_nodes; // maps node to depth
    std::stack<GraphNode*>                        stack_path_nodes;
    std::stack<GraphNode*>                        stack_explore_nodes;
    std::stack<std::pair<GNiterator, GNiterator>> stack_explore_pairs;

    // dfs init
    seen_nodes.emplace(start_node, timestamp++);
    stack_path_nodes.emplace(start_node);
    stack_explore_nodes.emplace(start_node);
    stack_explore_pairs.emplace(start_node->get_random_neighbors_iterators());

    // iterative dfs
    while (!stack_explore_nodes.empty() && !stack_explore_pairs.empty())
    {
        // check if there's anything more to see from stack_explore_graph_nodes.top() node
        if (stack_explore_pairs.top().first == stack_explore_pairs.top().second)
        {
            // found SCC
            if (stack_path_nodes.top() == stack_explore_nodes.top())
                stack_path_nodes.top()->find_set()->mark_as_dead(),
                // std::cerr << "marked as dead = " << stack_path_nodes.top()->find_set()->get_label() << std::endl,
                stack_path_nodes.pop();
            stack_explore_nodes.pop();
            stack_explore_pairs.pop();

            continue;
        }

        GraphNode* next_node = *stack_explore_pairs.top().first;
        ++stack_explore_pairs.top().first;

        if (!next_node->find_set()->is_dead())
        {
            // if next_node has been seen, we found cycle!
            if (seen_nodes.find(next_node) != seen_nodes.end())
            {
                while (seen_nodes.at(next_node) < seen_nodes.at(stack_path_nodes.top()))
                {
                    GraphNode* to_merge = stack_path_nodes.top();
                    stack_path_nodes.pop();

                    // union_set can return false if it failed
                    while (!to_merge->union_set(stack_path_nodes.top()));
                }
            }
            // next_node has not been seen. "recursively" call on it
            else
            {
                seen_nodes.emplace(next_node, timestamp++);
                stack_path_nodes.emplace(next_node);
                stack_explore_nodes.emplace(next_node);
                stack_explore_pairs.emplace(next_node->get_random_neighbors_iterators());
            }
        }
    }
}
