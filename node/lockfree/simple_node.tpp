simple_node::simple_node()
: _spin_lock(false), _dead(false), _parent(this), _mask(0), _size(1)
{
}

// template<typename node>
// simple_node<node>::simple_node()
// : _spin_lock(false), _dead(false), _parent(this), _mask(0), _size(1)
// {
// }

simple_node*
simple_node::find_set() const
{
    simple_node* me           = this;
    simple_node* parent       = _parent.load();
    simple_node* grand_parent = nullptr;

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

// template<typename node>
// simple_node<node>*
// simple_node<node>::find_set()
// {
//     simple_node* me = this;
//     simple_node* parent = _parent.load();

//     while (me != parent)
//     {
//         simple_node* grand_parent = parent->_parent.load();

//         me->_parent.compare_exchange_strong(parent, grand_parent);
//         me = parent;
//         parent = grand_parent;
//     }

//     return me;
// }

bool
simple_node::same_set(simple_node const * other) const
{
    simple_node* me_repr    = find_set();
    simple_node* other_repr = other->find_set();

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

// template<typename node>
// bool
// simple_node<node>::same_set(simple_node* other)
// {
//     return find_set() == other->find_set();
// }

bool
simple_node::has_mask(uint64_t mask) const
{
    return ((_mask.load()) & mask) != 0;
}

bool
simple_node::is_dead() const
{
    return _dead.load();
}

// template<typename node>
// bool
// simple_node<node>::is_dead()
// {
//     return _dead.load();
// }

bool
simple_node::union_set(simple_node* other)
{
    simple_node* me_repr    = find_set();
    simple_node* other_repr = other->find_set();
    bool         success    = false;

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr->lock())
    {
        if (other_repr->lock())
        {
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

// template<typename node>
// bool
// simple_node<node>::union_set(simple_node* other)
// {
//     simple_node* me_repr = find_set();
//     simple_node* other_repr = other->find_set();

//     while (me_repr != other_repr)
//     {
//         bool me_repr_lock_state = false;
//         bool other_repr_lock_state = false;

//         if (!(me_repr->_lock.compare_exchange_strong(me_repr_lock_state, true))) // fail
//         {
//             return false;
//         }
//         else // success
//         {
//             if (!(other_repr->_lock.compare_exchange_strong(other_repr_lock_state, true))) // fail
//             {
//                 me_repr_lock_state = true;
//                 me_repr->_lock.compare_exchange_strong(me_repr_lock_state, false);
//                 return false;
//             }
//             else // success
//             {
//                 if (_size.load() < other_repr->_size.load())
//                 {
//                     if (me_repr->_parent.compare_exchange_strong(me_repr, other_repr))
//                         other_repr->_size = other_repr->_size.load() + me_repr->_size.load();

//                     me_repr_lock_state = true;
//                     me_repr->_lock.compare_exchange_strong(me_repr_lock_state, false);

//                     other_repr_lock_state = true;
//                     other_repr->_lock.compare_exchange_strong(other_repr_lock_state, false);
//                 }
//                 else
//                 {
//                     if (other_repr->_parent.compare_exchange_strong(other_repr, me_repr))
//                         me_repr->_size = me_repr->_size.load() + other_repr->_size.load();

//                     me_repr_lock_state = true;
//                     me_repr->_lock.compare_exchange_strong(me_repr_lock_state, false);

//                     other_repr_lock_state = true;
//                     other_repr->_lock.compare_exchange_strong(other_repr_lock_state, false);
//                 }
//             }
//         }
//     }

//     return true;
// }

void
simple_node::add_mask(uint64_t mask)
{
    simple_node* repr = find_set();

    do
    {
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top())
}

void
simple_node::mark_as_dead()
{
    _dead.store(true);
}

// template<typename node>
// void
// simple_node<node>::mark_as_dead()
// {
//     _dead.store(true);
// }

bool
simple_node::is_top() const
{
    return _parent.load() == this;
}

bool
simple_node::lock()
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

void
simple_node::unlock()
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

void
simple_node::hook_under_me(simple_node* other)
{
    // update parent
    other->_parent.compare_exchange_strong(other, this);

    // update size
    _size += other->_size.load();

    // update mask
    _mask.fetch_or(other->_mask.load());
}
