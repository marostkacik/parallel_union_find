#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::lockfree
{
class simple_union_node
{
public:
    simple_union_node();

    // observer
    simple_union_node* find_set() const;
    bool               same_set(simple_union_node const *) const;
    bool               has_mask(uint64_t) const;
    bool               is_dead() const;

    // mutators
    bool               union_set(simple_union_node*);
    void               add_mask(uint64_t);
    void               mark_as_dead();

private:
    bool               is_top() const;

    bool               lock();
    void               unlock();

    void               hook_under_me(simple_union_node* other);

private:
    std::atomic<bool>                       _spin_lock;
    std::atomic<bool>                       _dead;

    // union set data
    mutable std::atomic<simple_union_node*> _parent;
    std::atomic<uint64_t>                   _mask;
    std::atomic<uint64_t>                   _size;
};

#include "simple_union_node.tpp"
}
