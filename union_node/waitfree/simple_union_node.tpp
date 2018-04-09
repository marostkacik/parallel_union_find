template<typename Derived>
BaseNode<Derived>::BaseNode()
: _dead(false), _parent(static_cast<Derived*>(this)), _mask(0)
{
}

template<typename Derived>
Derived*
BaseNode<Derived>::find_set() const
{
    Derived* me           = static_cast<Derived*>(const_cast<BaseNode<Derived>*>(this));
    Derived* parent       = _parent.load();
    Derived* grand_parent = nullptr;

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

template<typename Derived>
bool
BaseNode<Derived>::same_set(Derived const * other) const
{
    Derived const * me_repr    = find_set();
    Derived const * other_repr = other->find_set();

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

template<typename Derived>
bool
BaseNode<Derived>::is_dead() const
{
    return _dead.load();
}

template<typename Derived>
bool
BaseNode<Derived>::union_set(Derived* other)
{
    Derived* me_repr    = find_set();
    Derived* other_repr = other->find_set();

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr < other_repr)
        return other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
    else
        return me_repr->_parent.compare_exchange_strong(me_repr, other_repr);
}

template<typename Derived>
void
BaseNode<Derived>::mark_as_dead()
{
    _dead.store(false);
}

template<typename Derived>
bool
BaseNode<Derived>::is_top() const
{
    return _parent.load() == static_cast<Derived*>(const_cast<BaseNode<Derived>*>(this));
}
