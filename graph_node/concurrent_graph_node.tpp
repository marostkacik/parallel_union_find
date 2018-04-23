template<typename union_node>
concurrent_graph_node<union_node>*
concurrent_graph_node<union_node>::find_set() const
{
    return static_cast<concurrent_graph_node<union_node>*>(union_node::find_set());
}

template<typename union_node>
concurrent_graph_node<union_node>*
concurrent_graph_node<union_node>::get_node_from_set() const
{
    return static_cast<concurrent_graph_node<union_node>*>(union_node::get_node_from_set());
}

template<typename union_node>
std::pair<typename concurrent_graph_node<union_node>::iterator, typename concurrent_graph_node<union_node>::iterator>
concurrent_graph_node<union_node>::get_random_neighbors_iterators()
{
    return {iterator(_neighbors, 0), iterator(_neighbors, _neighbors.size())};
}

template<typename union_node>
void
concurrent_graph_node<union_node>::add_son(concurrent_graph_node<union_node>* node)
{
    _neighbors.push_back(node);
}

template<typename union_node>
void
concurrent_graph_node<union_node>::set_label(size_t label)
{
    _label = label;
}

template<typename union_node>
size_t
concurrent_graph_node<union_node>::get_label() const
{
    return _label;
}

template<typename union_node>
concurrent_graph_node_iterator<union_node>::concurrent_graph_node_iterator(const std::vector<concurrent_graph_node<union_node>*>& vector, size_t steps)
: _vector(vector), _next_pos(), _steps(steps)
{
    if (_vector.size() == 0)
        _next_pos = 0;
    else
        _next_pos = (reinterpret_cast<size_t>(this) * 397) % _vector.size();
}

template<typename union_node>
bool
concurrent_graph_node_iterator<union_node>::operator==(const concurrent_graph_node_iterator<union_node>& other) const
{
    return _steps == other._steps;
}

template<typename union_node>
concurrent_graph_node_iterator<union_node>&
concurrent_graph_node_iterator<union_node>::operator++()
{
    _next_pos = (_next_pos + 1) % _vector.size();
    _steps++;

    return *this;
}

template<typename union_node>
concurrent_graph_node<union_node>*
concurrent_graph_node_iterator<union_node>::operator*() const
{
    return _vector.at(_next_pos);
}
