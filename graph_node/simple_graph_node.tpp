template<typename node>
simple_graph_node_iterator<node>&
simple_graph_node_iterator<node>::operator++()
{
    _act_pos++;
    _steps_done++;

    if (_act_pos == _simple_graph_node->_neighbors.size())
        _act_pos = 0;
}

template<typename node>
simple_graph_node_iterator<node>
simple_graph_node_iterator<node>::operator++(int)
{
    simple_graph_node_iterator<node> previous_state = *this;

    this->operator++();

    return previous_state;
}

template<typename node>
bool
simple_graph_node_iterator<node>::operator==(const simple_graph_node_iterator<node>& other)
{
    return _act_pos == other._act_pos && _steps_done == other._steps_done;
}

template<typename node>
bool
simple_graph_node_iterator<node>::operator!=(const simple_graph_node_iterator<node>& other)
{
    return !(*this == other);
}

template<typename node>
simple_graph_node<node>*
simple_graph_node_iterator<node>::operator*()
{
    return _simple_graph_node->_neighbors.at(_act_pos);
}

template<typename node>
simple_graph_node<node>::simple_graph_node(node* n)
: _node(n), _next_state(0)
{
}

template<typename node>
std::pair<typename simple_graph_node<node>::iterator, typename simple_graph_node<node>::iterator>
simple_graph_node<node>::get_random_neighbors_iterators()
{
    simple_graph_node<node>::iterator start;
    simple_graph_node<node>::iterator finish;
    uint8_t                           state = _next_state.fetch_add(1);

    start._simple_graph_node  = this;
    start._steps_done         = 0;

    finish._simple_graph_node = this;
    finish._steps_done        = _neighbors.size();

    if (_neighbors.size() == 0)
        start._act_pos  = 0,
        finish._act_pos = 0;
    else if (state % 3 == 0)
        start._act_pos  = 0,
        finish._act_pos = 0;
    else if (state % 3 == 1)
        start._act_pos  = _neighbors.size() / 2,
        finish._act_pos = _neighbors.size() / 2;
    else if (state % 3 == 2)
        start._act_pos  = _neighbors.size() - 1,
        finish._act_pos = _neighbors.size() - 1;

    return {start, finish};
}

template<typename node>
void
simple_graph_node<node>::add_son(simple_graph_node<node>* new_son)
{
    _neighbors.push_back(new_son);
}
