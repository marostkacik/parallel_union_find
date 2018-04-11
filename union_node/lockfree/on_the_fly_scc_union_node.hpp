#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::lockfree
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
        bool  has_mask(uint64_t) const;
        bool  is_dead() const;
        bool  is_done() const;
        Node* get_node_from_set() const;

        // mutators
        bool  union_set(Node*);
        void  add_mask(uint64_t);
        void  mark_as_dead();
        void  mark_as_done();

    private:
        bool  is_top() const;

        bool  lock() const;
        void  unlock() const;

        void  hook_under_me(Node* other);

    private:
        mutable std::atomic<bool>  _spin_lock;
        std::atomic<bool>          _dead;
        std::atomic<bool>          _done;

        // union set data
        mutable std::atomic<Node*> _parent;
        std::atomic<uint64_t>      _mask;
        std::atomic<uint64_t>      _size;

        // circular linked list data
        mutable std::atomic<Node*> _start_node;
        mutable std::atomic<Node*> _next_node;
    };
}
using on_the_fly_scc_union_node = Node;

#include "on_the_fly_scc_union_node.tpp"
}
