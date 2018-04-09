template<typename Derived>
BaseNode<Derived>::BaseNode()
: _spin_lock(false), _dead(false), _parent(static_cast<Derived*>(this)), _mask(0), _size(1)
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
BaseNode<Derived>::has_mask(uint64_t mask) const
{
    return ((_mask.load()) & mask) != 0;
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
    bool     success    = false;

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr->lock())
    {
        if (other_repr->lock())
        {
            // now me_repr and other_repr cannot be changed
            if (me_repr->is_top() && other_repr->is_top())
            {
                if (me_repr->_size.load() >= other_repr->_size.load())
                    me_repr->hook_under_me(other_repr);
                else
                    other_repr->hook_under_me(me_repr);

                success = true;
            }
            other_repr->unlock();
        }
        me_repr->unlock();
    }

    return success;
}

template<typename Derived>
void
BaseNode<Derived>::add_mask(uint64_t mask)
{
    Derived* repr = find_set();

    do
    {
        repr = repr->find_set();
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top());
}

template<typename Derived>
void
BaseNode<Derived>::mark_as_dead()
{
    _dead.store(true);
}

template<typename Derived>
bool
BaseNode<Derived>::is_top() const
{
    return _parent.load() == this;
}

template<typename Derived>
bool
BaseNode<Derived>::lock() const
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

template<typename Derived>
void
BaseNode<Derived>::unlock() const
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

template<typename Derived>
void
BaseNode<Derived>::hook_under_me(Derived* other)
{
    // update parent
    other->_parent.compare_exchange_strong(other, static_cast<Derived*>(this));

    // update size
    _size += other->_size.load();

    // update mask
    _mask.fetch_or(other->_mask.load());
}
