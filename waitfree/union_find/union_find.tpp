union_find::union_find()
: _allocator(), _allocation_mutex(), _array(), _size(0), _capacity(0)
{
}

node*
union_find::at(uint64_t pos)
{

}

std::pair<uint64_t, uint64_t>
union_find::_index_to_pos(uint64_t pos)
{
    if (pos == 0)
        return {0, 0};

    uint64_t pos_first_1 = 64 - __builtin_clzll(pos);
    uint64_t mask = ((uint64_t)1 << (pos_first_1 - 1)) - 1;

    return {pos_first_1, (pos & mask)};
}

