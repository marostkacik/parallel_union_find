#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::union_node::waitfree
{
class simple_union_node
{
public:
    simple_union_node();

    // observers
    simple_union_node* find_set() const;
    bool               same_set(simple_union_node const *) const;
    bool               is_dead() const;

    // mutators
    bool               union_set(simple_union_node*);
    bool               mark_as_dead();

private:
    bool               is_top() const;

private:
    std::atomic<bool>                       _dead;

    // union set data
    mutable std::atomic<simple_union_node*> _parent;
};

#include "simple_union_node.tpp"
}
