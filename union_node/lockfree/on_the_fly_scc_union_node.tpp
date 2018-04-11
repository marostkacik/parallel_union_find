Node::Node()
: _spin_lock(false), _dead(false), _done(false), _parent(this), _mask(0), _size(1), _start_node(this), _next_node(this)
{
}

Node*
Node::find_set() const
{
    Node* me           = const_cast<Node*>(this);
    Node* parent       = _parent.load();
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

bool
Node::same_set(Node const * other) const
{
    Node const * me_repr    = find_set();
    Node const * other_repr = other->find_set();

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
Node::has_mask(uint64_t mask) const
{
    return ((_mask.load()) & mask) != 0;
}

bool
Node::is_dead() const
{
    return _dead.load();
}

bool
Node::is_done() const
{
    return _done.load();
}

Node*
Node::get_node_from_set() const
{
    Node* act  = _start_node.load();
    Node* next = nullptr;

    // grab act node only for yourself
    do
    {
        if (act == nullptr)
            return nullptr;
        else
            next = act->_next_node.load();
    } while (!_start_node.compare_exchange_strong(act, next));

    // try to pop next node if it's done
    if (next->_done.load())
        if (this->lock())
        {
            if (act == next)
            {
                // check again whether no new node was added
                if (_start_node.load() == _start_node.load()->_next_node.load() && _start_node.load()->is_done())
                    _start_node.store(nullptr);
            }
            else
                act->_next_node.compare_exchange_strong(next, next->_next_node.load());

            this->unlock();
        }

    return act;
}

bool
Node::union_set(Node* other)
{
    Node* me_repr    = find_set();
    Node* other_repr = other->find_set();
    bool  success    = false;

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
Node::add_mask(uint64_t mask)
{
    Node* repr = find_set();

    do
    {
        repr = repr->find_set();
        repr->_mask.fetch_or(mask);
    } while (!repr->is_top());
}

bool
Node::mark_as_dead()
{
    bool expected = false;
    return _dead.compare_exchange_strong(expected, true);
}

bool
Node::mark_as_done()
{
    bool expected = false;
    return _dead.compare_exchange_strong(expected, true);
}

bool
Node::is_top() const
{
    return _parent.load() == this;
}

bool
Node::lock() const
{
    bool expected = false;
    return _spin_lock.compare_exchange_strong(expected, true);
}

void
Node::unlock() const
{
    bool expected = true;
    _spin_lock.compare_exchange_strong(expected, false);
}

void
Node::hook_under_me(Node* other)
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
        Node* new_top_1 = _start_node.load();
        Node* new_top_2 = new_top_1->_next_node.load();
        Node* other_1   = other->_start_node.load();
        Node* other_2   = other_1->_next_node.load();

        // rewire cycle
        new_top_1->_next_node.store(other_2);
        other_1->_next_node.store(new_top_2);
    }
}
