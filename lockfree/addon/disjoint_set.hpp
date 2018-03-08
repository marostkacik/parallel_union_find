#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::lockfree::addon
{
    class disjoint_set
    {
        public:
            disjoint_set();

            disjoint_set* find_set();
            bool same_set(disjoint_set*);
            bool union_set(disjoint_set*);
        private:
            std::atomic<bool> _lock;
            std::atomic<uint64_t> _size;
            std::atomic<disjoint_set*> _parent;
    };

    #include "disjoint_set.tpp"
}
