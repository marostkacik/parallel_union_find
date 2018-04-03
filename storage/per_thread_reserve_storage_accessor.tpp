per_thread_reserve_storage_accessor::per_thread_reserve_storage_accessor(per_thread_reserve_storage& ptrs, uint8_t thread_id)
: _ptrs(ptrs), _thread_id(thread_id)
{
}

node*
per_thread_reserve_storage_accessor::at(uint64_t x)
{
    return _ptrs.at(x);
}

void
per_thread_reserve_storage_accessor::resize(uint64_t x)
{
    return _ptrs.resize(x);
}

uint64_t
per_thread_reserve_storage_accessor::size() const
{
    return _ptrs.size();
}

uint64_t
per_thread_reserve_storage_accessor::capacity() const
{
    return _ptrs.capacity();
}

void
per_thread_reserve_storage_accessor::reserve_exclusive_segment(uint64_t length)
{
    return _ptrs.reserve_exclusive_segment(_thread_id, length);
}

uint64_t
per_thread_reserve_storage_accessor::get_new_free_position()
{
    return _ptrs.get_new_free_position(_thread_id);
}

uint64_t
per_thread_reserve_storage_accessor::how_many_left_positions()
{
    return _ptrs.how_many_left_positions(_thread_id);
}
