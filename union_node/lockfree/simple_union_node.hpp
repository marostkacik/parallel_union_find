#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::lockfree
{
struct simple_union_node
{
public:
    simple_union_node();

    // observers
    template<typename Node>
    static Node* find_set(Node const *);
    template<typename Node>
    static bool  same_set(Node const *, Node const *);
    template<typename Node>
    static bool  has_mask(Node const *, uint64_t);
    template<typename Node>
    static bool  is_dead(Node const *);

    // mutators
    template<typename Node>
    static bool  union_set(Node*, Node*);
    template<typename Node>
    static void  add_mask(Node*, uint64_t);
    template<typename Node>
    static bool  mark_as_dead(Node*);

private:
    // helper functions
    template<typename Node>
    static bool  is_top(Node const *);
    template<typename Node>
    static bool  lock(Node const *);
    template<typename Node>
    static void  unlock(Node const *);
    template<typename Node>
    static void  hook_under_me(Node*, Node*);

private:
    mutable std::atomic<bool>               _spin_lock;
    std::atomic<bool>                       _dead;

    // union set data
    mutable std::atomic<simple_union_node*> _parent;
    std::atomic<uint64_t>                   _mask;
    std::atomic<uint64_t>                   _size;
};

#include "simple_union_node.tpp"
}
