#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::node::lockfree
{
class on_the_fly_scc_node
{
public:
    on_the_fly_scc_node();

    // observers
    on_the_fly_scc_node* find_set() const;
    bool                 same_set(on_the_fly_scc_node const *) const;
    bool                 has_mask(uint64_t) const;
    bool                 is_dead() const;
    on_the_fly_scc_node* get_node_from_set() const;

    // mutators
    bool                 union_set(on_the_fly_scc_node*);
    void                 add_mask(uint64_t);
    void                 mark_as_dead();

private:
    bool                 is_top() const;

    bool                 lock();
    bool                 unlock();

    void                 hook_under_me(on_the_fly_scc_node* other);

private:
    std::atomic<bool>                         _spin_lock;
    std::atomic<bool>                         _dead;

    // union set data
    mutable std::atomic<on_the_fly_scc_node*> _parent;
    std::atomic<uint64_t>                     _mask;
    std::atomic<uint64_t>                     _size;

    // circular linked list data
    mutable std::atomic<on_the_fly_scc_node*> _start_node;
    mutable std::atomic<on_the_fly_scc_node*> _next_node;
};

#include "on_the_fly_scc_union_node.tpp"
}
