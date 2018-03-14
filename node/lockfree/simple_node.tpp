simple_node::simple_node()
: _lock(false), _size(1), _parent(this)
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
