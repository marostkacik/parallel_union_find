#pragma once

#include <bits/stdc++.h>
#include "simple_storage.hpp"

namespace parallel_union_find::storage
{
template<typename node>
class per_thread_reserve_storage : public simple_storage<node>
{
public:
    per_thread_reserve_storage();

    void     reserve_exclusive_segment(uint8_t thread_id, uint64_t length);
    uint64_t get_new_free_position(uint8_t thread_id);
    uint64_t how_many_left_positions(uint8_t thread_id);

private:
    std::atomic<uint64_t>    _first_free_global_position;
    std::array<uint64_t, 64> _next_local_position;
    std::array<uint64_t, 64> _left_local_positions;
};

#include "per_thread_reserve_storage.tpp"
}
