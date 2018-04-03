per_thread_storage::per_thread_storage()
: _first_free_global_position(0)
{
    _next_local_position.fill(0);
    _left_local_positions.fill(0);
}

uint64_t
per_thread_storage::reserve_exclusive_segment(uint8_t thread_id, uint64_t length)
{
    uint64_t start_id = _first_free_global_position.load();

    while (!_first_free_global_position.compare_exchange_strong(start_id, start_id + length));

    if (capacity() < start_id + length)
        resize(start_id + length);

    _next_local_position.at(thread_id) = start_id;
    _left_local_positions.at(thread_id) = length;
}

uint64_t
per_thread_storage::get_new_free_position(uint8_t thread_id)
{
    if (_left_local_positions.at(thread_id) == 0)
        reserve_exclusive_segment(thread_id, 1000);

    _left_local_positions.at(thread_id)--;
    return _next_local_position.at(thread_id)++;
}

uint64_t
per_thread_storage::how_many_left_positions(uint8_t thread_id)
{
    return _left_local_positions.at(thread_id);
}
