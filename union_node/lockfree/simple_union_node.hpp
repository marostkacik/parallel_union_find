#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::lockfree
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
        bool     has_mask(uint64_t) const;
        bool     is_dead() const;

        // mutators
        bool     union_set(Derived*);
        void     add_mask(uint64_t);
        void     mark_as_dead();

    private:
        bool     is_top() const;

        bool     lock() const;
        void     unlock() const;

        void     hook_under_me(Derived* other);

    private:
        mutable std::atomic<bool>     _spin_lock;
        std::atomic<bool>             _dead;

        // union set data
        mutable std::atomic<Derived*> _parent;
        std::atomic<uint64_t>         _mask;
        std::atomic<uint64_t>         _size;
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
