simple_union_node::simple_union_node()
: _spin_lock(false), _dead(false), _parent(this), _mask(0), _merged_top(nullptr), _size(1)
{
}

simple_union_node*
simple_union_node::find_set() const
{
    simple_union_node* me           = const_cast<simple_union_node*>(this);
    simple_union_node* parent       = _parent.load();
    simple_union_node* grand_parent = nullptr;

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
simple_union_node::same_set(simple_union_node const * other) const
{
    simple_union_node const * me_repr    = find_set();
    simple_union_node const * other_repr = other->find_set();

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
simple_union_node::has_mask(uint64_t mask) const
{
    simple_union_node* repr       = find_set();
    simple_union_node* merged_top = repr->_merged_top.load();

    bool repr_valid       = (repr->_mask.load() & mask) != 0;
    bool merged_top_valid = merged_top && (merged_top->_mask.load() & mask) != 0;

    return repr_valid || merged_top_valid;
}

bool
simple_union_node::is_dead() const
{
    return find_set()->_dead.load();
}

bool
simple_union_node::union_set(simple_union_node* other)
{
    simple_union_node* me_repr    = find_set();
    simple_union_node* other_repr = other->find_set();
    bool  success                 = false;

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

void
simple_union_node::add_mask(uint64_t mask)
{
    simple_union_node* repr = find_set();

    do
    {
        repr = repr->find_set();
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top());
}

bool
simple_union_node::mark_as_dead()
{
    simple_union_node* repr = find_set();
    bool               success;

    do
    {
        repr = repr->find_set();

        bool expected = false;
        success = repr->_dead.compare_exchange_strong(expected, true);
    } while (!repr->is_top());

    return success;
}

bool
simple_union_node::is_top() const
{
    return _parent.load() == this;
}

bool
simple_union_node::lock() const
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

void
simple_union_node::unlock() const
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

void
simple_union_node::hook_under_me(simple_union_node* other)
{
    // set merged node
    _merged_top.store(other);

    // update parent
    other->_parent.compare_exchange_strong(other, this);

    // update size
    _size += other->_size.load();

    // update mask
    _mask.fetch_or(other->_mask.load());

    // remove merged node, now mask and parent are ok
    _merged_top.store(nullptr);
}
