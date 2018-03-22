//******
//*Node*
//******
template<typename R>
Node<R>::Node(R * const r)
: _union_node(this), _list_node(this), _spin_lock(false), _representative(r)
{
}

template<typename R>
Node<R>*
Node<R>::find_set()
{
    return _union_node.find_set();
}

template<typename R>
bool
Node<R>::same_set(Node<R>* other)
{
    return _union_node.same_set(other);
}

template<typename R>
bool
Node<R>::union_set(Node<R>* other)
{
    Node<R>* me_repr = find_set();
    Node<R>* other_repr = other->find_set();
    bool success = false;

    if (same_set(other))
        return true;

    if (me_repr->lock())
    {
        if (other_repr->lock())
        {
            // now me_repr and other_repr cannot be changed
            if (me_repr->_union_node._parent.load() == me_repr && other_repr->_union_node._parent.load() == other_repr)
            {
                if (me_repr->_union_node._size >= other_repr->_union_node._size)
                    me_repr->_union_node.merge_set(other_repr),
                    me_repr->_list_node.append_list(other_repr);
                else
                    other_repr->_union_node.merge_set(me_repr),
                    other_repr->_list_node.append_list(me_repr);

                success = true;
            }
            other_repr->unlock();
        }
        me_repr->unlock();
    }

    return success;
}

template<typename R>
void
Node<R>::add_mask(uint64_t mask)
{
    _union_node.add_mask(mask);
}

template<typename R>
uint64_t
Node<R>::get_mask()
{
    return _union_node._mask.load();
}

template<typename R>
size_t
Node<R>::active_list_length()
{
    return _list_node._length.load();
}

template<typename R>
Node<R>*
Node<R>::get_random_active_node(size_t seed)
{
    return ListNode<R>::get_node(this, seed);
}

template<typename R>
R*
Node<R>::get_representative()
{
    return _representative;
}

template<typename R>
void
Node<R>::mark_as_dead()
{
    _list_node._dead.store(false);
}

template<typename R>
bool
Node<R>::lock()
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

template<typename R>
void
Node<R>::unlock()
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

//***********
//*UnionNode*
//***********
template<typename R>
UnionNode<R>::UnionNode(Node<R>* parent)
: _parent(parent), _mask(0), _size(1)
{
}

template<typename R>
Node<R>*
UnionNode<R>::find_set()
{
    Node<R>* me_repr      = _parent.load();
    Node<R>* parent_repr  = me_repr->_union_node._parent.load();
    Node<R>* grand_parent = nullptr;

    while (me_repr != parent_repr)
    {
        // find grandparent
        grand_parent = parent_repr->_union_node._parent.load();

        // update parent
        me_repr->_union_node._parent.compare_exchange_strong(parent_repr, grand_parent);
        // update mask set
        parent_repr->_union_node._mask.fetch_or(me_repr->_union_node._mask.load());

        // move up
        me_repr = parent_repr;
        parent_repr = grand_parent;
    }

    return me_repr;
}

template<typename R>
bool
UnionNode<R>::same_set(Node<R>* other)
{
    Node<R>* me_repr = find_set();
    Node<R>* other_repr = other->find_set();

    while (true)
        if (me_repr == other_repr)
            return true;
        else if (me_repr != me_repr->_union_node._parent.load())
            me_repr = me_repr->find_set();
        else if (other_repr != other_repr->_union_node._parent.load())
            other_repr = other_repr->find_set();
        else
            return false;
}

template<typename R>
void
UnionNode<R>::merge_set(Node<R>* other)
{
    // set other's parent
    other->_union_node._parent.store(_parent.load());
    // update mask
    _mask.fetch_or(other->_union_node._mask.load());
    // update size
    _size += other->_union_node._size;
}

template<typename R>
void
UnionNode<R>::add_mask(uint64_t mask)
{
    _mask.fetch_or(mask);
    find_set();
}

//**********
//*ListNode*
//**********
template<typename R>
ListNode<R>::ListNode(Node<R>* start_node)
: _start_node(start_node), _last_node(start_node), _next_node(nullptr), _prev_node(nullptr), _dead(false), _length(1)
{
}

template<typename R>
void
ListNode<R>::append_list(Node<R>* other)
{
    if (!_start_node.load() && !_last_node.load()) // we have empty list
    {
        _start_node.store(other->_list_node._start_node.load()); // simply copy values from other node
        _last_node.store(other->_list_node._last_node.load());
    }
    else if (other->_list_node._start_node.load() && other->_list_node._last_node.load())
    {
        _last_node.load()->_list_node._next_node.store(other->_list_node._start_node.load()); // set continuation
        other->_list_node._start_node.load()->_list_node._prev_node.store(_last_node.load()); // set previous for starting appended node
        _last_node.store(other->_list_node._last_node.load()); // shift last node pointer
    }

    // update length
    _length.fetch_add(other->_list_node._length.load());
}

template<typename R>
Node<R>*
ListNode<R>::shift_to_next_active(Node<R>* head_node, Node<R>* act_node)
{
    while(act_node && act_node->_list_node._dead.load())
    {
        // try to pop this node
        bool expected = false;
        if (head_node->_spin_lock.compare_exchange_strong(expected, true))
        {
            // check if head_node is still head node
            if (head_node->_union_node._parent.load() == head_node)
            {
                // act_node is first
                if (head_node->_list_node._start_node.load() == act_node)
                    head_node->_list_node._start_node.store(act_node->_list_node._next_node.load());
                // act_node is last
                if (head_node->_list_node._last_node.load() == act_node)
                    head_node->_list_node._last_node.store(act_node->_list_node._prev_node.load());

                // update previous and next
                if (act_node->_list_node._prev_node.load())
                    act_node->_list_node._prev_node.load()->_list_node._next_node.store(act_node->_list_node._next_node.load());
                if (act_node->_list_node._next_node.load())
                    act_node->_list_node._next_node.load()->_list_node._prev_node.store(act_node->_list_node._prev_node.load());

                // update length of this list
                head_node->_list_node._length.fetch_sub(1);
            }

            // unlock spin lock
            expected = true;
            head_node->_spin_lock.compare_exchange_strong(expected, false);
        }

        // move next
        act_node = act_node->_list_node._next_node.load();
    }

    return act_node;
}

template<typename R>
Node<R>*
ListNode<R>::get_node(Node<R>* head_node, size_t n)
{
    Node<R>* act_node = head_node->_list_node._start_node.load();
    Node<R>* const first_ok = shift_to_next_active(head_node, act_node);
    act_node = first_ok;

    for (size_t i = 0; i < n && act_node; ++i)
        act_node = act_node->_list_node._next_node.load(),
        act_node = shift_to_next_active(head_node, act_node);

    if (!act_node)
        return first_ok;
    else
        return act_node;
}
