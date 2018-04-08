template<typename GraphNode>
void
simple_algorithm(GraphNode* start_node)
{
    using GNiterator = typename GraphNode::iterator;

    std::unordered_set<GraphNode*>                seen_nodes;
    std::stack<GraphNode*>                        stack_path_graph_nodes;
    std::stack<std::pair<GNiterator, GNiterator>> stack_explore_pairs;
    std::stack<GraphNode*>                        stack_explore_graph_nodes;

    // dfs init
    seen_nodes.insert(start_node);
    stack_path_graph_nodes.emplace(start_node);
    stack_explore_pairs.emplace(start_node->get_random_neighbors_iterators());
    stack_explore_graph_nodes.emplace(start_node);

    // iterative dfs
    while (!stack_explore_pairs.empty() && !stack_explore_graph_nodes.empty())
    {
        // check if there's anything more to see from stack_explore_graph_nodes.top() node
        if (stack_explore_pairs.top().first == stack_explore_pairs.top().second)
        {
            stack_explore_graph_nodes.top()->mark_as_dead();

            seen_nodes.erase(stack_explore_graph_nodes.top());
            if (stack_path_graph_nodes.top() == stack_explore_graph_nodes.top())
                stack_path_graph_nodes.pop();
            stack_explore_pairs.pop();
            stack_explore_graph_nodes.pop();

            continue;
        }

        GraphNode* next_graph_node = *stack_explore_pairs.top().first;
        ++stack_explore_pairs.top().first;

        if (!next_graph_node->is_dead())
        {
            // if next_graph_node has been seen, we found cycle!
            if (seen_nodes.find(next_graph_node) != seen_nodes.end())
            {
                GraphNode* act_graph_node_repr  = stack_explore_graph_nodes.top();
                GraphNode* next_graph_node_repr = next_graph_node;

                while (!act_graph_node_repr->same_set(next_graph_node_repr))
                {
                    GraphNode* to_merge1 = stack_path_graph_nodes.top(); stack_path_graph_nodes.pop();
                    GraphNode* to_merge2 = stack_path_graph_nodes.top();

                    // union can return false if operation failed
                    while (!to_merge1->union_set(to_merge2));
                }
            }
            // next_graph_node has not been seen, "recursively" call on it
            else
            {
                seen_nodes.insert(next_graph_node);
                stack_path_graph_nodes.emplace(next_graph_node);
                stack_explore_pairs.emplace(next_graph_node->get_random_neighbors_iterators());
                stack_explore_graph_nodes.emplace(next_graph_node);
            }
        }
    }
}
