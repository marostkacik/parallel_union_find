template<typename node>
simple_storage<node>::simple_storage()
: _allocation_mutex(), _array(), _used(), _size(0), _capacity(0), _layer(0)
{
}

template<typename node>
simple_storage<node>::~simple_storage()
{
    uint64_t first_unused_layer = _layer.load();

    for (uint64_t layer = 0; layer < first_unused_layer; ++layer)
        delete[] _array.at(layer);
}

template<typename node>
node*
simple_storage<node>::at(uint64_t index)
{
    std::pair<uint64_t, uint64_t> pos = _index_to_pos(index);
    return _array.at(pos.first) + pos.second;
}

template<typename node>
void
simple_storage<node>::resize(uint64_t new_size)
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
    while (new_size > old_size)
        _size.compare_exchange_weak(old_size, new_size);
}

template<typename node>
uint64_t
simple_storage<node>::size() const
{
    return _size.load();
}

template<typename node>
uint64_t
simple_storage<node>::capacity() const
{
    return _capacity.load();
}

template<typename node>
std::vector<std::vector<node*>>
simple_storage<node>::report_components()
{
    const uint64_t                      upper_bound = _capacity.load();
    std::unordered_map<node*, uint64_t> representative_to_idx;
    std::vector<std::vector<node*>>     answer;

    for (uint64_t i = 0; i < upper_bound; ++i)
        if (_is_used(i))
        {
            node* const act_node       = at(i);
            node* const representative = act_node->find_set();

            if (representative_to_idx.at(representative) == representative_to_idx.end())
                representative_to_idx.emplace(representative, representative_to_idx.size()),
                answer.emplace_back();

            const uint64_t idx = representative_to_idx.at(representative);
            answer.at(idx).push_back(act_node);
        }

    return answer;
}

template<typename node>
void
simple_storage<node>::mark_as_used(uint64_t index)
{
    std::pair<uint64_t, uint64_t> pos = _index_to_pos(index);
    _used.at(pos.first).at(pos.second) = true;
}

template<typename node>
void
simple_storage<node>::_add_new_layer()
{
    uint64_t new_layer_idx = _layer.fetch_add(1);
    uint64_t add_capacity = (_capacity.load() != 0) ? _capacity.load() : 1;

    // allocate add_capacity nodes without calling constructor
    _array.at(new_layer_idx) = static_cast<node*>(operator new(sizeof(node) * add_capacity));
    _used.at(new_layer_idx).resize(add_capacity, false);

    // update capacity
    _capacity.store(_capacity.load() + add_capacity);
}

template<typename node>
bool
simple_storage<node>::_is_used(uint64_t index)
{
    std::pair<uint64_t, uint64_t> pos = _index_to_pos(index);
    return _used.at(pos.first).at(pos.second);
}

template<typename node>
std::pair<uint64_t, uint64_t>
simple_storage<node>::_index_to_pos(uint64_t pos)
{
    if (pos == 0)
        return {0, 0};

    uint64_t pos_first_1 = 64 - __builtin_clzll(pos);
    uint64_t mask = ((uint64_t)1 << (pos_first_1 - 1)) - 1;

    return {pos_first_1, (pos & mask)};
}
