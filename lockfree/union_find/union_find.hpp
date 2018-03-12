#pragma once

#include <bits/stdc++.h>
#include "lockfree/addon/disjoint_set.hpp"

namespace parallel_union_find::lockfree::data_structure
{
    class disjoint_set
    {
        public:
            disjoint_set();



            void resize();
            uint64_t capacity() const;
        protected:

    };

    #include "disjoint_set.tpp"
}
