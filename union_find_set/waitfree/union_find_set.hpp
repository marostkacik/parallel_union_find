#pragma once

#include <atomic>

namespace parallel_union_find::union_find_set::waitfree
{
    class union_find_set
    {
    public:
        union_find_set();

        // observers
        union_find_set* find_set() const;
        bool            same_set(union_find_set const *) const;
        bool            is_dead() const;

        // mutators
        bool            union_set(union_find_set*);
        bool            mark_as_dead();

    private:
        bool            is_top() const;

    private:
        std::atomic<bool>                    _dead;

        // union set data
        mutable std::atomic<union_find_set*> _parent;
    };

#include "union_find_set.tpp"
}
