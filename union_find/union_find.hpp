#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find
{
    template<typename node>
    class union_find
    {
        public:
            union_find();
            ~union_find();

            node* at(uint64_t);

            void resize(uint64_t);
            uint64_t size() const;
            uint64_t capacity() const;
        private:
            std::mutex _allocation_mutex;
            std::array<node*, 64 + 1> _array;

            std::atomic<uint64_t> _size;
            std::atomic<uint64_t> _capacity;
            std::atomic<uint64_t> _layer;

            void _add_new_layer();
            static std::pair<uint64_t, uint64_t> _index_to_pos(uint64_t);
    };

    #include "union_find.tpp"
}
