#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::waitfree
{
namespace
{
    class Node
    {
    public:
        Node();

        // observers
        Node* find_set() const;
        bool  same_set(Node const *) const;
        bool  is_dead() const;

        // mutators
        bool  union_set(Node*);
        bool  mark_as_dead();

    private:
        bool  is_top() const;

    private:
        std::atomic<bool>          _dead;

        // union set data
        mutable std::atomic<Node*> _parent;
        std::atomic<uint64_t>      _mask;
    };
}

using simple_union_node = Node;

#include "simple_union_node.tpp"
}
