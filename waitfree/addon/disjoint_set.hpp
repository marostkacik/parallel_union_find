#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::waitfree::addon
{
    class disjoint_set
    {
        public:
            disjoint_set();

            disjoint_set* find_set();
            bool same_set(disjoint_set*);
            void union_set(disjoint_set*);
        private:
            std::atomic<disjoint_set*> _parent;
    };

    #include "disjoint_set.tpp"
}
