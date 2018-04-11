Node::Node()
: _dead(false), _parent(this), _mask(0)
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
Node::is_dead() const
{
    return _dead.load();
}

bool
Node::union_set(Node* other)
{
    Node* me_repr    = find_set();
    Node* other_repr = other->find_set();

    if (me_repr->same_set(other_repr))
        return true;

    if (me_repr < other_repr)
        return other_repr->_parent.compare_exchange_strong(other_repr, me_repr);
    else
        return me_repr->_parent.compare_exchange_strong(me_repr, other_repr);
}

bool
Node::mark_as_dead()
{
    bool expected = false;
    _dead.compare_exchange_strong(expected, true);
}

bool
Node::is_top() const
{
    return _parent.load() == this;
}
