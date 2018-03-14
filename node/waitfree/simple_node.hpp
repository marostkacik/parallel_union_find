#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::waitfree
{
    class simple_node
    {
        public:
            simple_node();

            simple_node* find_set();
            bool same_set(simple_node*);
            bool union_set(simple_node*);
        private:
            std::atomic<simple_node*> _parent;
    };

    #include "simple_node.tpp"
}
