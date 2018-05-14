#pragma once

#include <atomic>
#include <tuple>
#include <cassert>

namespace parallel_union_find::union_node::lockfree
{
    class on_the_fly_scc_union_node
    {
    public:
        on_the_fly_scc_union_node();

        // observers
        on_the_fly_scc_union_node* find_set() const; // wait-free, returns representative
        std::pair<bool, bool>      same_set(on_the_fly_scc_union_node const *) const; // lock-free, returns <value, success>
        bool                       has_mask(uint64_t) const; // wait-free, returns value
        bool                       is_dead() const; // wait-free, returns value
        bool                       is_done() const; // wait-free, returns value
        on_the_fly_scc_union_node* get_node_from_set() const; // wait-free, returns node

        // mutators
        bool                       union_set(on_the_fly_scc_union_node*); // lock-free, returns success
        bool                       add_mask(uint64_t); // lock-free, returns success
        bool                       mark_as_dead(); // wait-free, returns whether this threat set value as first
        bool                       mark_as_done(); // wait-free, returns whether this threat set value as first

    private:
        bool                       is_top() const; // wait-free, returns value

        bool                       lock() const; // wait-free, returns value
        void                       unlock() const; // wait-free, returns nothing

        void                       block(); // wait-free, returns nothing
        void                       unblock(); // wait-free, returns nothing
        bool                       is_blocked() const; // wait-free, returns value

        on_the_fly_scc_union_node* get_node_from_set_not_locking(); // wait-free, returns node
        void                       hook_under_me(on_the_fly_scc_union_node* other); // wait-free, returns nothing

    private:
        mutable std::atomic<bool>                       _lock;
        std::atomic<bool>                               _block;
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
