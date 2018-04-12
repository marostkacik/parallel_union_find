simple_union_node::simple_union_node()
: _spin_lock(false), _dead(false), _parent(this), _mask(0), _size(1)
{
}

template<typename Node>
Node*
simple_union_node::find_set(Node const * obj)
{
    Node* me           = const_cast<Node*>(obj);
    Node* parent       = obj->_parent.load();
    Node* grand_parent = nullptr;

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

template<typename Node>
bool
simple_union_node::same_set(Node const * obj, Node const * other)
{
    Node const * me_repr    = find_set(obj);
    Node const * other_repr = find_set(other);

    while (true)
        if (me_repr == other_repr)
            return true;
        else if (!is_top(me_repr))
            me_repr = find_set(me_repr);
        else if (!is_top(other_repr))
            other_repr = find_set(other_repr);
        else
            return false;
}

template<typename Node>
bool
simple_union_node::has_mask(Node const * obj, uint64_t mask)
{
    return ((obj->_mask.load()) & mask) != 0;
}

template<typename Node>
bool
simple_union_node::is_dead(Node const * obj)
{
    return obj->_dead.load();
}

template<typename Node>
bool
simple_union_node::union_set(Node* obj, Node* other)
{
    Node* me_repr    = find_set(obj);
    Node* other_repr = find_set(other);
    bool  success    = false;

    if (same_set(me_repr, other_repr))
        return true;

    if (lock(me_repr))
    {
        if (lock(other_repr))
        {
            // now me_repr and other_repr cannot be changed
            if (is_top(me_repr) && is_top(other_repr))
            {
                if (me_repr->_size.load() >= other_repr->_size.load())
                    hook_under_me(me_repr, other_repr);
                else
                    hook_under_me(other_repr, me_repr);

                success = true;
            }
            unlock(other_repr);
        }
        unlock(me_repr);
    }

    return success;
}

template<typename Node>
void
simple_union_node::add_mask(Node* obj, uint64_t mask)
{
    Node* repr = find_set(obj);

    do
    {
        repr = find_set(repr);
        repr->_mask.fetch_or(mask);
    } while (!is_top(repr));
}

template<typename Node>
bool
simple_union_node::mark_as_dead(Node* obj)
{
    bool expected = false;
    return obj->_dead.compare_exchange_strong(expected, true);
}

template<typename Node>
bool
simple_union_node::is_top(Node const * obj)
{
    return obj->_parent.load() == obj;
}

template<typename Node>
bool
simple_union_node::lock(Node const * obj)
{
    bool expected = false;
    return obj->_spin_lock.compare_exchange_strong(expected, true);
}

template<typename Node>
void
simple_union_node::unlock(Node const * obj)
{
    bool expected = true;
    obj->_spin_lock.compare_exchange_strong(expected, false);
}

template<typename Node>
void
simple_union_node::hook_under_me(Node* obj, Node* other)
{
    // update parent
    other->_parent.compare_exchange_strong(other, obj);

    // update size
    obj->_size += other->_size.load();

    // update mask
    obj->_mask.fetch_or(other->_mask.load());
}
