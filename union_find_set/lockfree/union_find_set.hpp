#pragma once

#include <atomic>

namespace parallel_union_find::union_find_set::lockfree
{
    class union_find_set
    {
    public:
        union_find_set();

        // observers
        union_find_set* find_set() const;
        bool            same_set(union_find_set const *) const;
        bool            has_mask(uint64_t) const;
        bool            is_dead() const;

        // mutators
        bool            union_set(union_find_set*);
        void            add_mask(uint64_t);
        bool            mark_as_dead();

    private:
        bool            is_top() const;

        bool            lock() const;
        void            unlock() const;

        void            hook_under_me(union_find_set* other);

    private:
        mutable std::atomic<bool>            _spin_lock;
        std::atomic<bool>                    _dead;

        // union set data
        mutable std::atomic<union_find_set*> _parent;
        std::atomic<uint64_t>                _mask;
        std::atomic<uint64_t>                _size;
    };

#include "union_find_set.tpp"
}
