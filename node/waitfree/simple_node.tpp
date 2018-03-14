simple_node::simple_node()
: _parent(this)
{
}

simple_node*
simple_node::find_set()
{
    simple_node* me = this;
    simple_node* parent = _parent.load();

    while (me != parent)
    {
        simple_node* grand_parent = parent->_parent.load();

        me->_parent.compare_exchange_strong(parent, grand_parent);
        me = parent;
        parent = grand_parent;
    }

    return me;
}

bool
simple_node::same_set(simple_node* other)
{
    return find_set() == other->find_set();
}

bool
simple_node::union_set(simple_node* other)
{
    simple_node* me_repr = find_set();
    simple_node* other_repr = other->find_set();

    while (me_repr != other_repr)
    {
        if (me_repr < other_repr)
            other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
        else
            me_repr->_parent.compare_exchange_strong(me_repr, other_repr);

        me_repr = me_repr->find_set();
        other_repr = other_repr->find_set();
    }

    return true;
}
