#pragma once

#include <atomic>
#include <cassert>

namespace parallel_union_find::union_node::lockfree
{
    class on_the_fly_scc_union_node
    {
    public:
        on_the_fly_scc_union_node();

        // observers
        on_the_fly_scc_union_node* find_set() const;
        bool                       same_set(on_the_fly_scc_union_node const *) const;
        bool                       has_mask(uint64_t) const;
        bool                       is_dead() const;
        bool                       is_done() const;
        on_the_fly_scc_union_node* get_node_from_set() const;

        // mutators
        bool                       union_set(on_the_fly_scc_union_node*);
        void                       add_mask(uint64_t);
        bool                       mark_as_dead();
        bool                       mark_as_done();

    private:
        bool                       is_top() const;

        bool                       lock() const;
        void                       unlock() const;

        on_the_fly_scc_union_node* get_node_from_set_not_locking();
        void                       hook_under_me(on_the_fly_scc_union_node* other);

    private:
        mutable std::atomic<bool>                       _spin_lock;
        std::atomic<bool>                               _dead;
        std::atomic<bool>                               _done;

        // union set data
        mutable std::atomic<on_the_fly_scc_union_node*> _parent;
        std::atomic<uint64_t>                           _mask;
        std::atomic<uint64_t>                           _size;
        std::atomic<bool>                               _blocked;

        // circular linked list data
        mutable std::atomic<on_the_fly_scc_union_node*> _start_node;
        mutable std::atomic<on_the_fly_scc_union_node*> _next_node;
    };

#include "on_the_fly_scc_union_node.tpp"
}
