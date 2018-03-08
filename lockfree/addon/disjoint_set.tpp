disjoint_set::disjoint_set()
: _lock(false), _size(1), _parent(this)
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

bool
disjoint_set::union_set(disjoint_set* other)
{
    disjoint_set* me_repr = find_set();
    disjoint_set* other_repr = other->find_set();

    while (me_repr != other_repr)
    {
        bool me_repr_lock_state = false;
        bool other_repr_lock_state = false;

        if (!(me_repr->_lock.compare_exchange_strong(me_repr_lock_state, true))) // fail
        {
            return false;
        }
        else // success
        {
            if (!(other_repr->_lock.compare_exchange_strong(other_repr_lock_state, true))) // fail
            {
                me_repr_lock_state = true;
                me_repr->_lock.compare_exchange_strong(me_repr_lock_state, false);
                return false;
            }
            else // success
            {
                if (_size.load() < other_repr->_size.load())
                {
                    if (me_repr->_parent.compare_exchange_strong(me_repr, other_repr))
                        other_repr->_size = other_repr->_size.load() + me_repr->_size.load();

                    me_repr_lock_state = true;
                    me_repr->_lock.compare_exchange_strong(me_repr_lock_state, false);

                    other_repr_lock_state = true;
                    other_repr->_lock.compare_exchange_strong(other_repr_lock_state, false);
                }
                else
                {
                    if (other_repr->_parent.compare_exchange_strong(other_repr, me_repr))
                        me_repr->_size = me_repr->_size.load() + other_repr->_size.load();

                    me_repr_lock_state = true;
                    me_repr->_lock.compare_exchange_strong(me_repr_lock_state, false);

                    other_repr_lock_state = true;
                    other_repr->_lock.compare_exchange_strong(other_repr_lock_state, false);
                }
            }
        }
    }

    return true;
}
