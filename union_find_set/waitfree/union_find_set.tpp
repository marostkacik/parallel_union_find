simple_union_node::simple_union_node()
: _dead(false), _parent(this)
{
}

simple_union_node*
simple_union_node::find_set() const
{
    simple_union_node* me           = const_cast<simple_union_node*>(this);
    simple_union_node* parent       = _parent.load();
    simple_union_node* grand_parent = nullptr;

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

bool
simple_union_node::same_set(simple_union_node const * other) const
{
    simple_union_node const * me_repr    = find_set();
    simple_union_node const * other_repr = other->find_set();

    while (true)
        if (me_repr == other_repr)
            return true;
        else if (!me_repr->is_top())
            me_repr = me_repr->find_set();
        else if (!other_repr->is_top())
            other_repr = other_repr->find_set();
        else
            return false;
}

bool
simple_union_node::is_dead() const
{
    return find_set()->_dead.load();
}

bool
simple_union_node::union_set(simple_union_node* other)
{
    simple_union_node* me_repr    = find_set();
    simple_union_node* other_repr = other->find_set();

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr < other_repr)
        return other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
    else
        return me_repr->_parent.compare_exchange_strong(me_repr, other_repr);
}

bool
simple_union_node::mark_as_dead()
{
    simple_union_node* repr = find_set();
    bool               success;

    do
    {
        repr = repr->find_set();

        bool expected = false;
        success = repr->_dead.compare_exchange_strong(expected, true);
    } while (!repr->is_top());

    return success;
}

bool
simple_union_node::is_top() const
{
    return _parent.load() == this;
}
