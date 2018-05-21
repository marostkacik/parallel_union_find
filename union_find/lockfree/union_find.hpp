#pragma once

#include <atomic>

namespace parallel_union_find::union_find::waitfree
{
    class union_find
    {
    public:
        union_find();

        // observers
        union_find* find_set() const;
        bool        same_set(union_find const *) const;
        bool        is_dead() const;

        // mutators
        bool        union_set(union_find*);
        bool        mark_as_dead();

    private:
        bool        is_top() const;

    private:
        std::atomic<bool>                _dead;

        // union set data
        mutable std::atomic<union_find*> _parent;
    };

#include "union_find.tpp"
}
