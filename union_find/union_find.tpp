template<typename node>
union_find<node>::union_find()
: _allocation_mutex(), _array(), _size(0), _capacity(0), _layer(0)
{
}

template<typename node>
node*
union_find<node>::at(uint64_t index)
{
    std::pair<uint64_t, uint64_t> pos = _index_to_pos(index);
    return _array.at(pos.first) + pos.second;
}

template<typename node>
union_find<node>::~union_find()
{
    uint64_t first_unused_layer = _layer.load();

    for (uint64_t layer = 0; layer < first_unused_layer; ++layer)
        delete[] _array.at(layer);
}

template<typename node>
void
union_find<node>::resize(uint64_t new_size)
{
    // live lock for allocation of new layers
    while (new_size > _capacity.load())
        if (_allocation_mutex.try_lock())
        {
            _add_new_layer();
            _allocation_mutex.unlock();
        }

    // just increase counter, no new layer is needed
    uint64_t old_size = _size.load();
    while (new_size < _size.load())
        _size.compare_exchange_weak(old_size, new_size);
}

template<typename node>
uint64_t
union_find<node>::size() const
{
    return _size.load();
}

template<typename node>
uint64_t
union_find<node>::capacity() const
{
    return _capacity.load();
}

template<typename node>
void
union_find<node>::_add_new_layer()
{
    uint64_t new_layer_idx = _layer.fetch_add(1);
    node* new_layer_pointer = nullptr;
    uint64_t add_capacity;

    // determine size of new layer
    add_capacity = _capacity.load();
    if (add_capacity == 0)
        add_capacity = 1;

    // allocate and construct layer of size `add_capacity`
    new_layer_pointer = new node[add_capacity];
    // add layer to array of layers
    _array.at(new_layer_idx) = new_layer_pointer;

    // update capacity
    _capacity.store(_capacity.load() + add_capacity);
}

template<typename node>
std::pair<uint64_t, uint64_t>
union_find<node>::_index_to_pos(uint64_t pos)
{
    if (pos == 0)
        return {0, 0};

    uint64_t pos_first_1 = 64 - __builtin_clzll(pos);
    uint64_t mask = ((uint64_t)1 << (pos_first_1 - 1)) - 1;

    return {pos_first_1, (pos & mask)};
}
