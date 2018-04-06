#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::node::waitfree
{
class simple_node
{
public:
    simple_node();

    // observers
    simple_node* find_set() const;
    bool         same_set(simple_node const *) const;
    bool         is_dead() const;

    // mutators
    bool         union_set(simple_node*);
    void         mark_as_dead();

private:
    bool         is_top() const;

private:
    std::atomic<bool>                 _dead;

    // union set data
    mutable std::atomic<simple_node*> _parent;
    std::atomic<uint64_t>             _mask;
};

    #include "simple_node.tpp"
}
