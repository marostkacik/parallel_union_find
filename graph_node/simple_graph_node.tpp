template<typename union_node>
simple_graph_node_iterator<union_node>::simple_graph_node_iterator(std::vector<simple_graph_node<union_node>*>& vec, size_t pos, size_t steps)
: _vec(vec), _pos(pos), _steps(steps)
{
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
    ++_pos;
    ++_steps;

    if (_pos == _vec.size())
        _pos = 0;

    return *this;
}

template<typename union_node>
simple_graph_node<union_node>*
simple_graph_node_iterator<union_node>::operator*() const
{
    return _vec.at(_pos);
}

template<typename union_node>
simple_graph_node<union_node>::simple_graph_node()
: _state(0), _neighbors(), _label(-1)
{
}

template<typename union_node>
std::pair<typename simple_graph_node<union_node>::iterator, typename simple_graph_node<union_node>::iterator>
simple_graph_node<union_node>::get_random_neighbors_iterators()
{
    uint8_t state = _state.fetch_add(1);

    if (state % 3 == 0)
        return {iterator(_neighbors, 0, 0), iterator(_neighbors, 0, _neighbors.size())};
    else if (state % 3 == 1)
        return {iterator(_neighbors, _neighbors.size() / 3, 0), iterator(_neighbors, _neighbors.size() / 3, _neighbors.size())};
    else if (state % 3 == 2)
        return {iterator(_neighbors, (_neighbors.size() / 3) * 2, 0), iterator(_neighbors, (_neighbors.size() / 3) * 2, _neighbors.size())};
}

template<typename union_node>
void
simple_graph_node<union_node>::add_son(simple_graph_node<union_node>* node)
{
    _neighbors.push_back(node);
}

template<typename union_node>
void
simple_graph_node<union_node>::set_label(int label)
{
    _label = label;
}

template<typename union_node>
int
simple_graph_node<union_node>::get_label() const
{
    return _label;
}
