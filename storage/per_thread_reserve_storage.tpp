template<typename node>
per_thread_reserve_storage<node>::per_thread_reserve_storage()
: _first_free_global_position(0)
{
    _next_local_position.fill(0);
    _left_local_positions.fill(0);
}

template<typename node>
void
per_thread_reserve_storage<node>::reserve_exclusive_segment(uint8_t thread_id, uint64_t length)
{
    uint64_t start_id = _first_free_global_position.load();

    while (!_first_free_global_position.compare_exchange_strong(start_id, start_id + length));

    if (simple_storage<node>::capacity() < start_id + length)
        simple_storage<node>::resize(start_id + length);

    _next_local_position.at(thread_id) = start_id;
    _left_local_positions.at(thread_id) = length;
}

template<typename node>
uint64_t
per_thread_reserve_storage<node>::how_many_left_positions(uint8_t thread_id)
{
    return _left_local_positions.at(thread_id);
}

template<typename node>
template<typename... Args>
uint64_t
per_thread_reserve_storage<node>::create_new_node(uint8_t thread_id, Args&&... args)
{
    if (_left_local_positions.at(thread_id) == 0)
        reserve_exclusive_segment(thread_id, 1000);

    uint64_t pos = _next_local_position.at(thread_id)++;
    _left_local_positions.at(thread_id)--;

    new (simple_storage<node>::at(pos)) node(std::forward<Args>(args)...);

    return pos;
}
