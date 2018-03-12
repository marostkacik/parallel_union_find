node::node()
: _parent(this)
{
}

node*
node::find_set()
{
    node* me = this;
    node* parent = _parent.load();

    while (me != parent)
    {
        node* grand_parent = parent->_parent.load();

        me->_parent.compare_exchange_strong(parent, grand_parent);
        me = parent;
        parent = grand_parent;
    }

    return me;
}

bool
node::same_set(node* other)
{
    return find_set() == other->find_set();
}

void
node::union_set(node* other)
{
    node* me_repr = find_set();
    node* other_repr = other->find_set();

    while (me_repr != other_repr)
    {
        if (me_repr < other_repr)
            other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
        else
            me_repr->_parent.compare_exchange_strong(me_repr, other_repr);

        me_repr = me_repr->find_set();
        other_repr = other_repr->find_set();
    }
}
