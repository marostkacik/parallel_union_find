union_find::union_find()
: _dead(false), _parent(this)
{
}

union_find*
union_find::find_set() const
{
    union_find* me           = const_cast<union_find*>(this);
    union_find* parent       = _parent.load();
    union_find* grand_parent = nullptr;

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
union_find::same_set(union_find const * other) const
{
    union_find const * me_repr    = find_set();
    union_find const * other_repr = other->find_set();

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
union_find::is_dead() const
{
    return find_set()->_dead.load();
}

bool
union_find::union_set(union_find* other)
{
    union_find* me_repr    = find_set();
    union_find* other_repr = other->find_set();

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr < other_repr)
        return other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
    else
        return me_repr->_parent.compare_exchange_strong(me_repr, other_repr);
}

bool
union_find::mark_as_dead()
{
    union_find* repr = find_set();
    bool        success;

    do
    {
        repr = repr->find_set();

        bool expected = false;
        success = repr->_dead.compare_exchange_strong(expected, true);
    } while (!repr->is_top());

    return success;
}

bool
union_find::is_top() const
{
    return _parent.load() == this;
}
