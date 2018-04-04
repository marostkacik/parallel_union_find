#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::node::lockfree
{
template<typename node>
class simple_node
{
public:
    simple_node(node*);

    simple_node* find_set();
    bool         same_set(simple_node*);
    bool         union_set(simple_node*);

    node*        get_representative();
    void         mark_as_dead();
    bool         is_dead();

private:
    node* const               _representative;
    std::atomic<bool>         _lock;
    std::atomic<bool>         _dead;
    std::atomic<uint64_t>     _size;
    std::atomic<simple_node*> _parent;
};

#include "simple_node.tpp"
}
