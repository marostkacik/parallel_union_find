template<typename union_node>
simple_graph_node<union_node>::simple_graph_node()
: _state(0), _neighbors(), _label(0)
{
}

template<typename union_node>
simple_graph_node<union_node>*
simple_graph_node<union_node>::find_set() const
{
    return static_cast<simple_graph_node<union_node>*>(union_node::find_set());
}

template<typename union_node>
simple_graph_node<union_node>*
simple_graph_node<union_node>::get_node_from_set() const
{
    return static_cast<simple_graph_node<union_node>*>(union_node::get_node_from_set());
}

template<typename union_node>
std::pair<typename simple_graph_node<union_node>::iterator, typename simple_graph_node<union_node>::iterator>
simple_graph_node<union_node>::get_random_neighbors_iterators()
{
    return {iterator(*this, 0), iterator(*this, _neighbors.size())};
}

template<typename union_node>
void
simple_graph_node<union_node>::add_son(simple_graph_node<union_node>* node)
{
    _neighbors.push_back(node);
}

template<typename union_node>
void
simple_graph_node<union_node>::set_label(size_t label)
{
    _label = label;
}

template<typename union_node>
size_t
simple_graph_node<union_node>::get_label() const
{
    return _label;
}

template<typename union_node>
simple_graph_node_iterator<union_node>::simple_graph_node_iterator(simple_graph_node<union_node>& sgn, size_t steps)
: _sgn(sgn), _steps(steps)
{
    if (steps != _sgn._neighbors.size())
        _next_pos = _sgn._state.fetch_add(1) % _sgn._neighbors.size();
}

template<typename union_node>
bool
simple_graph_node_iterator<union_node>::operator==(const simple_graph_node_iterator<union_node>& other) const
{
    return _steps == other._steps;
}

template<typename union_node>
simple_graph_node_iterator<union_node>&
simple_graph_node_iterator<union_node>::operator++()
{
    _next_pos = _sgn._state.fetch_add(1) % _sgn._neighbors.size();
    _steps++;

    return *this;
}

template<typename union_node>
simple_graph_node<union_node>*
simple_graph_node_iterator<union_node>::operator*() const
{
    return _sgn._neighbors.at(_next_pos);
}
