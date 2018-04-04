#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::node::lockfree
{
class simple_node
{
public:
    simple_node();

    simple_node* find_set();
    bool same_set(simple_node*);
    bool union_set(simple_node*);
private:
    std::atomic<bool> _lock;
    std::atomic<uint64_t> _size;
    std::atomic<simple_node*> _parent;
};

#include "simple_node.tpp"
}
