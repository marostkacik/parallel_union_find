on_the_fly_scc_union_node::on_the_fly_scc_union_node()
: _spin_lock(false), _dead(false), _done(false), _parent(this), _mask(0), _size(1), _start_node(this), _next_node(this)
{
}

on_the_fly_scc_union_node*
on_the_fly_scc_union_node::find_set() const
{
    on_the_fly_scc_union_node* me           = const_cast<on_the_fly_scc_union_node*>(this);
    on_the_fly_scc_union_node* parent       = _parent.load();
    on_the_fly_scc_union_node* grand_parent = nullptr;

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
on_the_fly_scc_union_node::same_set(on_the_fly_scc_union_node const * other) const
{
    on_the_fly_scc_union_node const * me_repr    = find_set();
    on_the_fly_scc_union_node const * other_repr = other->find_set();

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
on_the_fly_scc_union_node::has_mask(uint64_t mask) const
{
    return (find_set()->_mask.load() & mask) != 0;
}

bool
on_the_fly_scc_union_node::is_dead() const
{
    return find_set()->_dead.load();
}

bool
on_the_fly_scc_union_node::is_done() const
{
    return _done.load();
}

on_the_fly_scc_union_node*
on_the_fly_scc_union_node::get_node_from_set() const
{
    on_the_fly_scc_union_node* act  = _start_node.load();
    on_the_fly_scc_union_node* next = nullptr;

    // grab act node only for yourself
    do
    {
        if (!act)
            return nullptr;
        else
            next = act->_next_node.load();
    } while (!_start_node.compare_exchange_strong(act, next));

    // we might want to pop next node
    if (next->is_done())
    {
        if (this->lock())
        {
            if (this->is_top())
            {
                on_the_fly_scc_union_node* last_node = _start_node.load();

                // last_node is last
                if (last_node && last_node == last_node->_next_node.load() && last_node->is_done())
                    _start_node.store(nullptr);
                // last node is either alive or is not last, so just make circle smaller, because next is done
                else
                    act->_next_node.compare_exchange_strong(next, next->_next_node.load());
            }
        }
        this->unlock();
    }

    return act;
}

bool
on_the_fly_scc_union_node::union_set(on_the_fly_scc_union_node* other)
{
    on_the_fly_scc_union_node* me_repr    = find_set();
    on_the_fly_scc_union_node* other_repr = other->find_set();
    bool  success                         = false;

    if (me_repr->same_set(other_repr))
        success = true;
    else if (me_repr->lock())
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
on_the_fly_scc_union_node::add_mask(uint64_t mask)
{
    on_the_fly_scc_union_node* repr = find_set();

    do
    {
        repr = repr->find_set();
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top());
}

bool
on_the_fly_scc_union_node::mark_as_dead()
{
    on_the_fly_scc_union_node* repr = find_set();
    bool                       success;

    do
    {
        repr = repr->find_set();

        bool expected = false;
        success = repr->_dead.compare_exchange_strong(expected, true);
    } while (!repr->is_top());

    return success;
}

bool
on_the_fly_scc_union_node::mark_as_done()
{
    bool expected = false;
    return _done.compare_exchange_strong(expected, true);
}

bool
on_the_fly_scc_union_node::is_top() const
{
    return _parent.load() == this;
}

bool
on_the_fly_scc_union_node::lock() const
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

void
on_the_fly_scc_union_node::unlock() const
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

on_the_fly_scc_union_node*
on_the_fly_scc_union_node::get_node_from_set_not_locking()
{
    on_the_fly_scc_union_node* act  = _start_node.load();
    on_the_fly_scc_union_node* next = nullptr;

    // grab act only for yourself
    do
    {
        if (!act)
            return nullptr;
        else
            next = act->_next_node.load();
    } while (!_start_node.compare_exchange_strong(act, next));

    // we might want to pop next node
    if (next->is_done())
    {
        on_the_fly_scc_union_node* last_node = _start_node.load();

        // last_node is last
        if (last_node && last_node == last_node->_next_node.load() && last_node->is_done())
            _start_node.store(nullptr);
        else
            act->_next_node.compare_exchange_strong(next, next->_next_node.load());
    }

    return act;
}

void
on_the_fly_scc_union_node::hook_under_me(on_the_fly_scc_union_node* other)
{
    // update data, mask has to go before parent, otherwise has_mask could incorrectly return answer
    _mask.fetch_or(other->_mask.load());
    _size += other->_size.load();
    other->_parent.compare_exchange_strong(other, this);

    on_the_fly_scc_union_node* new_top_1 = this->get_node_from_set_not_locking();
    on_the_fly_scc_union_node* new_top_2 = nullptr;
    on_the_fly_scc_union_node* other_1   = other->get_node_from_set_not_locking();
    on_the_fly_scc_union_node* other_2   = nullptr;

    while (new_top_1 && new_top_1->is_done())
        new_top_1 = this->get_node_from_set_not_locking();
    while (other_1 && other_1->is_done())
        other_1   = other->get_node_from_set_not_locking();

    if (new_top_1)
        new_top_2 = new_top_1->_next_node.load();
    if (other_1)
        other_2 = other_1->_next_node.load();

    // rewire cycle
    if (new_top_1 && other_1)
        new_top_1->_next_node.store(other_2),
        other_1->_next_node.store(new_top_2);
    else if (new_top_1 && !other_1);
    else if (!new_top_1 && other_1)
        _start_node.store(other_1);
    else if (!new_top_1 && !other_1);
}
