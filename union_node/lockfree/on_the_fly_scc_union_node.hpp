#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::lockfree
{
struct on_the_fly_scc_union_node
{
public:
    on_the_fly_scc_union_node();

    // observers
    template<typename Node>
    static Node* find_set(Node const *);

    template<typename Node>
    static bool same_set(Node const *, Node const *);

    template<typename Node>
    static bool has_mask(Node const *, uint64_t);

    template<typename Node>
    static bool is_dead(Node const *);

    template<typename Node>
    static bool is_done(Node const *);

    template<typename Node>
    static Node* get_node_from_set(Node const *);

    // mutators
    template<typename Node>
    static bool union_set(Node*, Node*);

    template<typename Node>
    static void add_mask(Node*, uint64_t);

    template<typename Node>
    static bool mark_as_dead(Node*);

    template<typename Node>
    static bool mark_as_done(Node*);

private:
    // helper functions
    template<typename Node>
    static bool is_top(Node const *);

    template<typename Node>
    static bool lock(Node const *);

    template<typename Node>
    static void unlock(Node const *);

    template<typename Node>
    static void hook_under_me(Node*, Node*);

private:
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

#include "on_the_fly_scc_union_node.tpp"
}
