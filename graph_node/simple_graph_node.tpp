template<typename union_node>
std::pair<typename simple_graph_node<union_node>::iterator, typename simple_graph_node<union_node>::iterator>
simple_graph_node<union_node>::get_random_neighbors_iterators()
{
    return {_neighbors.begin(), _neighbors.end()};
}

template<typename union_node>
void
simple_graph_node<union_node>::add_son(simple_graph_node<union_node>* node)
{
    _neighbors.push_back(node);
}
