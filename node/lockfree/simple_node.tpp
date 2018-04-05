template<typename node>
simple_node<node>::simple_node(node* repr)
: _representative(repr), _lock(false), _dead(false), _size(1), _parent(this)
{
}

template<typename node>
simple_node<node>*
simple_node<node>::find_set()
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

template<typename node>
bool
simple_node<node>::same_set(simple_node* other)
{
    return find_set() == other->find_set();
}

template<typename node>
bool
simple_node<node>::union_set(simple_node* other)
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

template<typename node>
node*
simple_node<node>::get_representative()
{
    return _representative;
}

template<typename node>
void
simple_node<node>::mark_as_dead()
{
    _dead.store(true);
}

template<typename node>
bool
simple_node<node>::is_dead()
{
    return _dead.load();
}
