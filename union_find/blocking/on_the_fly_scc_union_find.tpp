on_the_fly_scc_union_find::on_the_fly_scc_union_find()
: _spin_lock(false), _dead(false), _done(false), _parent(this), _mask(0), _size(1), _blocked(false), _start_node(this), _next_node(this)
{
}

on_the_fly_scc_union_find*
on_the_fly_scc_union_find::find_set() const
{
    on_the_fly_scc_union_find* me           = const_cast<on_the_fly_scc_union_find*>(this);
    on_the_fly_scc_union_find* parent       = _parent.load();
    on_the_fly_scc_union_find* grand_parent = nullptr;

    while (me != parent)
    {
        // find grandparent
        grand_parent = parent->_parent.load();

        // update data
        me->_parent.compare_exchange_strong(parent, grand_parent);
        parent->_mask.fetch_or(me->_mask.load());

        // move up
        me     = parent;
        parent = grand_parent;
    }

    return me;
}

bool
on_the_fly_scc_union_find::same_set(on_the_fly_scc_union_find const * other) const
{
    on_the_fly_scc_union_find const * me_repr    = find_set();
    on_the_fly_scc_union_find const * other_repr = other->find_set();

    while (true)
        if (!me_repr->_blocked.load() && !other_repr->_blocked.load() && me_repr->is_top() && other_repr->is_top() && me_repr == other_repr)
            return true;
        else if (!me_repr->is_top())
            me_repr = me_repr->find_set();
        else if (!other_repr->is_top())
            other_repr = other_repr->find_set();
        else if (!me_repr->_blocked.load() && !other_repr->_blocked.load() && me_repr->is_top() && other_repr->is_top() && me_repr != other_repr)
            return false;
}

bool
on_the_fly_scc_union_find::has_mask(uint64_t mask) const
{
    return (find_set()->_mask.load() & mask) != 0;
}

bool
on_the_fly_scc_union_find::is_dead() const
{
    return find_set()->_dead.load();
}

bool
on_the_fly_scc_union_find::is_done() const
{
    return _done.load();
}

on_the_fly_scc_union_find*
on_the_fly_scc_union_find::get_node_from_set() const
{
    on_the_fly_scc_union_find* act  = _start_node.load();
    on_the_fly_scc_union_find* next = nullptr;

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
                if (act->_next_node.compare_exchange_strong(next, next->_next_node.load()))
                    if (act == next)
                        _start_node.store(nullptr);

            this->unlock();
        }
    }

    return act;
}

bool
on_the_fly_scc_union_find::union_set(on_the_fly_scc_union_find* other)
{
    on_the_fly_scc_union_find* me_repr    = find_set();
    on_the_fly_scc_union_find* other_repr = other->find_set();
    bool  success                         = false;

    if (me_repr->same_set(other_repr))
        success = true;
    else if (me_repr->lock())
    {
        if (other_repr->lock())
        {
            if (me_repr == other_repr)
                success = true;
            else if (me_repr->is_top() && other_repr->is_top())
            {
                if (me_repr->_size.load() >= other_repr->_size.load())
                    me_repr->hook_under_me(other_repr);
                else
                    other_repr->hook_under_me(me_repr);

                success = true;

                assert(me_repr->same_set(other_repr));
            }

            other_repr->unlock();
        }
        me_repr->unlock();
    }

    return success;
}

void
on_the_fly_scc_union_find::add_mask(uint64_t mask)
{
    on_the_fly_scc_union_find* repr = find_set();

    do
    {
        repr = repr->find_set();
        repr->_mask.fetch_or(mask);
    } while (repr->_blocked.load() || !repr->is_top());

    assert(this->has_mask(mask));
}

bool
on_the_fly_scc_union_find::mark_as_dead()
{
    on_the_fly_scc_union_find* repr = find_set();
    bool                       success;

    do
    {
        repr = repr->find_set();

        bool expected = false;
        success = repr->_dead.compare_exchange_strong(expected, true);
    } while (!repr->is_top());

    assert(this->is_dead());

    return success;
}

bool
on_the_fly_scc_union_find::mark_as_done()
{
    bool expected = false;
    return _done.compare_exchange_strong(expected, true);
}

bool
on_the_fly_scc_union_find::is_top() const
{
    return _parent.load() == this;
}

bool
on_the_fly_scc_union_find::lock() const
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

void
on_the_fly_scc_union_find::unlock() const
{
    bool expected = true;

    assert(_spin_lock.load());

    _spin_lock.compare_exchange_strong(expected, false);
}

on_the_fly_scc_union_find*
on_the_fly_scc_union_find::get_node_from_set_not_locking()
{
    on_the_fly_scc_union_find* act  = _start_node.load();
    on_the_fly_scc_union_find* next = nullptr;

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
        if (act->_next_node.compare_exchange_strong(next, next->_next_node.load()))
            if (act == next)
                _start_node.store(nullptr);
    }

    return act;
}

void
on_the_fly_scc_union_find::hook_under_me(on_the_fly_scc_union_find* other)
{
    this->_blocked.store(true);
    other->_blocked.store(true);

    assert(!this->is_dead());
    assert(!other->is_dead());

    // update data, mask has to go before parent, otherwise has_mask could incorrectly return answer
    _mask.fetch_or(other->_mask.load());
    _size += other->_size.load();
    other->_parent.compare_exchange_strong(other, this);

    on_the_fly_scc_union_find* new_top_1 = this->get_node_from_set_not_locking();
    on_the_fly_scc_union_find* new_top_2 = nullptr;
    on_the_fly_scc_union_find* other_1   = other->get_node_from_set_not_locking();
    on_the_fly_scc_union_find* other_2   = nullptr;

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
    else if (new_top_1 && !other_1)
    {
    }
    else if (!new_top_1 && other_1)
        _start_node.store(other_1);
    else if (!new_top_1 && !other_1)
    {
    }

    this->_blocked.store(false);
    other->_blocked.store(false);
}
