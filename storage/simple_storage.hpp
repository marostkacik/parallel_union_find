#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::storage
{
template<typename node>
class simple_storage
{
public:
    simple_storage();
    ~simple_storage();

    node* at(uint64_t);

    void     resize(uint64_t);
    uint64_t size() const;
    uint64_t capacity() const;

    std::vector<std::vector<node*>> report_components();

protected:
    void mark_as_used(uint64_t);

private:
    std::mutex                            _allocation_mutex;
    std::array<node*, 64 + 1>             _array;
    std::array<std::vector<bool>, 64 + 1> _used;

    std::atomic<uint64_t> _size;
    std::atomic<uint64_t> _capacity;
    std::atomic<uint64_t> _layer;

    void _add_new_layer();
    bool _is_used(uint64_t index);
    static std::pair<uint64_t, uint64_t> _index_to_pos(uint64_t);
};

#include "simple_storage.tpp"
}
