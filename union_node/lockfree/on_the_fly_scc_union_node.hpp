#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::lockfree
{
struct on_the_fly_scc_union_node
{
    on_the_fly_scc_union_node();

    mutable std::atomic<bool>                       _spin_lock;
    std::atomic<bool>                               _dead;
    std::atomic<bool>                               _done;

    // union set data
    mutable std::atomic<on_the_fly_scc_union_node*> _parent;
    std::atomic<uint64_t>                           _mask;
    std::atomic<uint64_t>                           _size;

    // circular linked list data
    mutable std::atomic<on_the_fly_scc_union_node*> _start_node;
    mutable std::atomic<on_the_fly_scc_union_node*> _next_node;
};

// observers
template<typename Node>
Node* find_set(Node const *);

template<typename Node>
bool same_set(Node const *, Node const *);

template<typename Node>
bool has_mask(Node const *, uint64_t);

template<typename Node>
bool is_dead(Node const *);

template<typename Node>
bool is_done(Node const *);

template<typename Node>
Node* get_node_from_set(Node const *);

// mutators
template<typename Node>
bool union_set(Node*, Node*);

template<typename Node>
void add_mask(Node*, uint64_t);

template<typename Node>
bool mark_as_dead(Node*);

template<typename Node>
bool mark_as_done(Node*);

// helper functions
template<typename Node>
bool is_top(Node const *);

template<typename Node>
bool lock(Node const *);

template<typename Node>
void unlock(Node const *);

template<typename Node>
void hook_under_me(Node*, Node*);

#include "on_the_fly_scc_union_node.tpp"
}
