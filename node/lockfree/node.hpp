#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::lockfree
{
    class node
    {
        public:
            node();

            node* find_set();
            bool same_set(node*);
            bool union_set(node*);
        private:
            std::atomic<bool> _lock;
            std::atomic<uint64_t> _size;
            std::atomic<node*> _parent;
    };

    #include "node.tpp"
}
