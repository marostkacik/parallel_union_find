union_find_set::union_find_set()
: _spin_lock(false), _dead(false), _parent(this), _mask(0), _size(1)
{
}

union_find_set*
union_find_set::find_set() const
{
    union_find_set* me           = const_cast<union_find_set*>(this);
    union_find_set* parent       = _parent.load();
    union_find_set* grand_parent = nullptr;

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
union_find_set::same_set(union_find_set const * other) const
{
    union_find_set const * me_repr    = find_set();
    union_find_set const * other_repr = other->find_set();

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
union_find_set::has_mask(uint64_t mask) const
{
    return (find_set()->_mask.load() & mask) != 0;
}

bool
union_find_set::is_dead() const
{
    return find_set()->_dead.load();
}

bool
union_find_set::union_set(union_find_set* other)
{
    union_find_set* me_repr    = find_set();
    union_find_set* other_repr = other->find_set();
    bool  success              = false;

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr->lock())
    {
        if (other_repr->lock())
        {
            if (me_repr->same_set(other_repr))
                success = true;
            else if (me_repr->is_top() && other_repr->is_top())
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

void
union_find_set::add_mask(uint64_t mask)
{
    union_find_set* repr = find_set();

    do
    {
        repr = repr->find_set();
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top());
}

bool
union_find_set::mark_as_dead()
{
    union_find_set* repr = find_set();
    bool            success;

    do
    {
        repr = repr->find_set();

        bool expected = false;
        success = repr->_dead.compare_exchange_strong(expected, true);
    } while (!repr->is_top());

    return success;
}

bool
union_find_set::is_top() const
{
    return _parent.load() == this;
}

bool
union_find_set::lock() const
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

void
union_find_set::unlock() const
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

void
union_find_set::hook_under_me(union_find_set* other)
{
    _mask.fetch_or(other->_mask.load());
    _size += other->_size.load();
    other->_parent.compare_exchange_strong(other, this);
}
