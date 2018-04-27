#pragma once

#include "per_thread_reserve_storage.hpp"

namespace parallel_union_find::storage
{
template<typename node>
class per_thread_reserve_storage_accessor
{
public:
    per_thread_reserve_storage_accessor(per_thread_reserve_storage<node>& ptrs, uint8_t thread_id);

    // simple storage functionality
    node* at(uint64_t);

    void     resize(uint64_t);
    uint64_t size() const;
    uint64_t capacity() const;

    // per thread storage functionality
    void     reserve_exclusive_segment(uint64_t length);
    uint64_t how_many_left_positions();

    template<typename... Args>
    uint64_t create_new_node(Args&&...);

private:
    per_thread_reserve_storage<node>& _ptrs;
    uint8_t                           _thread_id;
};

#include "per_thread_reserve_storage_accessor.tpp"
}
