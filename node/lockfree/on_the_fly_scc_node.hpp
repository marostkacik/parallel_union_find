#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::node::lockfree
{
template<typename R>
class Node
{
public:
    Node(R*);

    Node<R>* find_set();
    bool     same_set(Node<R>* other);
    bool     union_set(Node<R>* other);

    void     add_mask(uint64_t mask);
    bool     has_mask(uint64_t mask);
    Node<R>* get_node_from_set();

    R*       get_representative();
    void     mark_as_dead();
    bool     is_dead();

private:
    bool lock();
    void unlock();

    bool is_top();
    void hook_under_me(Node<R>* other);

private:
    R* const              _representative;
    std::atomic<bool>     _spin_lock;

    std::atomic<Node<R>*> _parent;
    std::atomic<uint64_t> _mask;
    std::atomic<uint64_t> _size;

    std::atomic<Node<R>*> _start_node;
    std::atomic<Node<R>*> _next_node;
    std::atomic<bool>     _dead;
};

template<typename R>
using on_the_fly_scc_node = Node<R>;

#include "on_the_fly_scc_node.tpp"
}
