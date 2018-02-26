disjoint_set::disjoint_set()
: _parent(this)
{
}

disjoint_set*
disjoint_set::find_set()
{
    disjoint_set* me = this;
    disjoint_set* parent = _parent.load();

    while (me != parent)
    {
        disjoint_set* grand_parent = parent->_parent.load();

        me->_parent.compare_exchange_strong(parent, grand_parent);
        me = parent;
        parent = grand_parent;
    }

    return me;
}

bool
disjoint_set::same_set(disjoint_set* other)
{
    return find_set() == other->find_set();
}

void
disjoint_set::union_set(disjoint_set* other)
{
    disjoint_set* me_repr = find_set();
    disjoint_set* other_repr = other->find_set();

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
