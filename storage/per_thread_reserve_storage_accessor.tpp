template<typename node>
per_thread_reserve_storage_accessor<node>::per_thread_reserve_storage_accessor(per_thread_reserve_storage<node>& ptrs, uint8_t thread_id)
: _ptrs(ptrs), _thread_id(thread_id)
{
}

template<typename node>
node*
per_thread_reserve_storage_accessor<node>::at(uint64_t x)
{
    return _ptrs.at(x);
}

template<typename node>
void
per_thread_reserve_storage_accessor<node>::resize(uint64_t x)
{
    return _ptrs.resize(x);
}

template<typename node>
uint64_t
per_thread_reserve_storage_accessor<node>::size() const
{
    return _ptrs.size();
}

template<typename node>
uint64_t
per_thread_reserve_storage_accessor<node>::capacity() const
{
    return _ptrs.capacity();
}

template<typename node>
void
per_thread_reserve_storage_accessor<node>::reserve_exclusive_segment(uint64_t length)
{
    return _ptrs.reserve_exclusive_segment(_thread_id, length);
}

template<typename node>
uint64_t
per_thread_reserve_storage_accessor<node>::how_many_left_positions()
{
    return _ptrs.how_many_left_positions(_thread_id);
}

template<typename node>
template<typename... Args>
uint64_t
per_thread_reserve_storage_accessor<node>::create_new_node(Args&&... args)
{
    return _ptrs.create_new_node(_thread_id, std::forward<Args>(args)...);
}
