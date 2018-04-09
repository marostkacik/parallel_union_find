#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::waitfree
{
namespace
{
    template<typename Derived>
    class BaseNode
    {
    public:
        BaseNode();

        // observers
        Derived* find_set() const;
        bool     same_set(Derived const *) const;
        bool     is_dead() const;

        // mutators
        bool     union_set(Derived*);
        void     mark_as_dead();

    private:
        bool     is_top() const;

    private:
        std::atomic<bool>                       _dead;

        // union set data
        mutable std::atomic<Derived*> _parent;
        std::atomic<uint64_t>                   _mask;
    };

    template<typename Derived>
    class Node : public BaseNode<Derived>
    {
    };

    template<>
    class Node<void> : public BaseNode<Node<void>>
    {
    };
}

template<typename Derived>
using simple_union_node = Node<Derived>;

#include "simple_union_node.tpp"
}
