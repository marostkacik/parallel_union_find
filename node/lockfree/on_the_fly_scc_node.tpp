on_the_fly_scc_node::on_the_fly_scc_node()
: _spin_lock(false), _parent(this), _mask(0), _size(1), _start_node(this), _next_node(this), _dead(false)
{
}

// template<typename R>
// Node<R>::Node(R* r)
// : _representative(r), _spin_lock(false), _parent(this), _mask(0), _size(1), _start_node(this), _next_node(this), _dead(false)
// {
// }

on_the_fly_scc_node*
on_the_fly_scc_node::find_set() const
{
    on_the_fly_scc_node me           = this;
    on_the_fly_scc_node parent       = _parent.load();
    on_the_fly_scc_node grand_parent = nullptr;

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

// template<typename R>
// Node<R>*
// Node<R>::find_set()
// {
//     Node<R>* me           = this;
//     Node<R>* parent       = _parent.load(std::memory_order_relaxed);
//     Node<R>* grand_parent = nullptr;

//     while (me != parent)
//     {
//         // find grandparent
//         grand_parent = parent->_parent.load(std::memory_order_relaxed);

//         // update data
//         me->_parent.compare_exchange_strong(parent, grand_parent, std::memory_order_relaxed);

//         // move up
//         me = parent;
//         parent = grand_parent;
//     }

//     return me;
// }

bool
on_the_fly_scc_node::same_set(on_the_fly_scc_node const * other) const
{
    on_the_fly_scc_node* me_repr    = find_set();
    on_the_fly_scc_node* other_repr = other->find_set();

    while (true)
        if (me_repr == other_repr)
            return true;
        else if (!me_repr->is_top())
            me_repr = me_repr->find_set();
        else if (!other_repr->is_top())
            other_repr->find_set();
        else
            return false;
}

// template<typename R>
// bool
// Node<R>::same_set(Node<R>* other)
// {
//     Node<R>* me_repr    = find_set();
//     Node<R>* other_repr = other->find_set();

//     while (true)
//         if (me_repr == other_repr)
//             return true;
//         else if (!me_repr->is_top())
//             me_repr = me_repr->find_set();
//         else if (!other_repr->is_top())
//             other_repr = other_repr->find_set();
//         else
//             return false;
// }

bool
on_the_fly_scc_node::has_mask(uint64_t mask) const
{
    return ((_mask.load()) & mask) != 0;
}

// template<typename R>
// bool
// Node<R>::has_mask(uint64_t mask)
// {
//     return (_mask.load() & mask) != 0;
// }

on_the_fly_scc_node*
on_the_fly_scc_node::get_node_from_set() const
{
    on_the_fly_scc_node* act  = _start_node.load();
    on_the_fly_scc_node* next = nullptr;

    // grab act node only for yourself
    do
    {
        if (act == nullptr)
            return nullptr;
        else
            next = act->_next_node.load();
    } while (!_start_node.compare_exchange_strong(act, next))

    // try to pop next node if it's dead
    if (next->_dead.load() && this->lock())
    {
        if (act == next)
        {
            // check again whether no new node was added
            if (_start_node.load() == _start_node.load()->_next_node.load() && _start_node.load()->_dead.load())
                _start_node.store(nullptr);
        }
        else
            act->_next_node.compare_exchange_strong(next, next->_next_node.load());

        this->unlock();
    }
}

// template<typename R>
// Node<R>*
// Node<R>::get_node_from_set()
// {
//     Node<R>* act  = _start_node.load();
//     Node<R>* next;

//     do
//     {
//         if (act == nullptr)
//             return nullptr;
//         else
//             next = act->_next_node.load();
//     } while(!_start_node.compare_exchange_strong(act, next));

//     if (next->_dead.load())
//     {
//         if (next == act)
//         {
//             if (this->lock())
//             {
//                 // check again whether no new node was added
//                 if (_start_node.load() == _start_node.load()->_next_node.load() && _start_node.load()->_dead.load())
//                     _start_node.store(nullptr);

//                 this->unlock();
//             }
//         }
//         else
//         {
//             if (this->lock())
//             {
//                 // now check it again and pop accordingly
//                 act->_next_node.compare_exchange_strong(next, next->_next_node.load());

//                 this->unlock();
//             }
//         }
//     }

//     return act;
// }

bool
on_the_fly_scc_node::union_set(on_the_fly_scc_node* other)
{
    on_the_fly_scc_node* me_repr    = find_set();
    on_the_fly_scc_node* other_repr = other->find_set();
    bool                 success    = false;

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

// template<typename R>
// bool
// Node<R>::union_set(Node<R>* other)
// {
//     Node<R>* me_repr    = find_set();
//     Node<R>* other_repr = other->find_set();

//     if (me_repr->same_set(other_repr))
//         return true;

//     bool success = false;
//     if (me_repr->lock())
//     {
//         if (other_repr->lock())
//         {
//             // now me_repr and other_repr cannot be changed
//             if (me_repr->is_top() && other_repr->is_top())
//             {
//                 if (me_repr->_size.load() >= other_repr->_size.load())
//                     me_repr->hook_under_me(other_repr);
//                 else
//                     other_repr->hook_under_me(me_repr);
//                 success = true;
//             }
//             other_repr->unlock();
//         }
//         me_repr->unlock();
//     }

//     return success;
// }

void
on_the_fly_scc_node::add_mask(uint64_t mask)
{
    on_the_fly_scc_node* repr = find_set();

    do
    {
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top());
}

// template<typename R>
// void
// Node<R>::add_mask(uint64_t mask)
// {
//     Node<R>* repr = find_set();

//     do {
//         repr->_mask.fetch_or(mask);
//     } while (!repr->is_top());
// }

void
on_the_fly_scc_node::mark_as_dead()
{
    _dead.store(true);
}

// template<typename R>
// void
// Node<R>::mark_as_dead()
// {
//     _dead.store(true);
// }

bool
on_the_fly_scc_node::is_top() const
{
    return _parent.load() == this;
}

// template<typename R>
// bool
// Node<R>::is_top()
// {
//     return _parent.load() == this;
// }

bool
on_the_fly_scc_node::is_dead() const
{
    return _dead.load();
}

// template<typename R>
// bool
// Node<R>::is_dead()
// {
//     return _dead.load();
// }

bool
on_the_fly_scc_node::lock()
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

// template<typename R>
// bool
// Node<R>::lock()
// {
//     bool expected = false;
//     return _spin_lock.compare_exchange_strong(expected, true);
// }

void
on_the_fly_scc_node::unlock()
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

// template<typename R>
// void
// Node<R>::unlock()
// {
//     bool expected = true;
//     _spin_lock.compare_exchange_strong(expected, false);
// }

void
on_the_fly_scc_node::hook_under_me(on_the_fly_scc_node* other)
{
    // update parent
    other->_parent.compare_exchange_strong(other, this);

    // update size
    _size += other->_size.load();

    // update mask
    _mask.fetch_or(other->_mask.load());

    // update list
    if (!_start_node.load())
        _start_node.store(other->_start_node.load());
    else if (other->_start_node.load())
    {
        on_the_fly_scc_node* new_top_1 = _start_node.load();
        on_the_fly_scc_node* new_top_2 = new_top_1->_next_node.load();
        on_the_fly_scc_node* other_1   = other->_start_node.load();
        on_the_fly_scc_node* other_2   = other_1->_next_node.load();

        // rewire cycle
        new_top_1->_next_node.store(other_2);
        other_1->_next_node.store(new_top_2);
    }
}

// template<typename R>
// void
// Node<R>::hook_under_me(Node<R>* other)
// {
//     // update parent
//     other->_parent.compare_exchange_strong(other, this);

//     // update size
//     _size += other->_size.load();

//     // update mask
//     _mask.fetch_or(other->_mask.load());

//     // update list
//     if (!_start_node.load())
//         _start_node.store(other->_start_node.load());
//     else if (other->_start_node.load())
//     {
//         Node<R>* newTop_1 = _start_node.load();
//         Node<R>* newTop_2 = newTop_1->_next_node.load();
//         Node<R>* other_1  = other->_start_node.load();
//         Node<R>* other_2  = other_1->_next_node.load();

//         // rewire cycle
//         newTop_1->_next_node.store(other_2);
//         other_1->_next_node.store(newTop_2);
//     }
// }
