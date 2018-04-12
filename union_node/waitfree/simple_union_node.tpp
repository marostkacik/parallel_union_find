simple_union_node::simple_union_node()
: _dead(false), _parent(this)
{
}

template<typename Node>
Node*
simple_union_node::find_set(Node const * obj)
{
    Node* me           = const_cast<Node*>(obj);
    Node* parent       = obj->_parent.load();
    Node* grand_parent = nullptr;

    while (me != parent)
    {
        // find grandparent
        grand_parent = parent->_parent.load();

        // update data
        me->_parent.compare_exchange_strong(parent, grand_parent);

        // move up
        me     = parent;
        parent = grand_parent;
    }

    return me;
}

template<typename Node>
bool
simple_union_node::same_set(Node const * obj, Node const * other)
{
    Node const * me_repr    = find_set(obj);
    Node const * other_repr = find_set(other);

    while (true)
        if (me_repr == other_repr)
            return true;
        else if (!is_top(me_repr))
            me_repr = find_set(me_repr);
        else if (!is_top(other_repr))
            other_repr = find_set(other_repr);
        else
            return false;
}

template<typename Node>
bool
simple_union_node::is_dead(Node const * obj)
{
    return obj->_dead.load();
}

template<typename Node>
bool
simple_union_node::union_set(Node* obj, Node* other)
{
    Node* me_repr    = find_set(obj);
    Node* other_repr = find_set(other);

    if (same_set(me_repr, other_repr))
        return true;

    if (me_repr < other_repr)
        return other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
    else
        return me_repr->_parent.compare_exchange_strong(me_repr, other_repr);
}

template<typename Node>
bool
simple_union_node::mark_as_dead(Node* obj)
{
    bool expected = false;
    return obj->_dead.compare_exchange_strong(expected, true);
}

template<typename Node>
bool
simple_union_node::is_top(Node const * obj)
{
    return obj->_parent.load() == obj;
}
