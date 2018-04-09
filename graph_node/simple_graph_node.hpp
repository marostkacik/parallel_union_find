#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::graph_node
{
template<typename union_node>
class simple_graph_node_iterator;

template<typename union_node>
class simple_graph_node;

template<typename union_node>
class simple_graph_node_iterator
{
public:
    simple_graph_node_iterator(std::vector<simple_graph_node<union_node>*>& vec, size_t pos, size_t steps);

    bool operator==(const simple_graph_node_iterator& other) const;
    simple_graph_node_iterator&    operator++();
    simple_graph_node<union_node>* operator*() const;

private:
    std::vector<simple_graph_node<union_node>*>& _vec;
    size_t                                       _pos;
    size_t                                       _steps;

};

template<typename union_node>
class simple_graph_node : public union_node
{
public:
    using iterator = simple_graph_node_iterator<union_node>;
    friend iterator;

public:
    simple_graph_node();

    std::pair<iterator, iterator> get_random_neighbors_iterators();
    void add_son(simple_graph_node<union_node>*);
    void set_label(int l);
    int  get_label() const;

private:
    std::atomic<uint8_t>                        _state;
    std::vector<simple_graph_node<union_node>*> _neighbors;
    int                                         _label;
};

#include "simple_graph_node.tpp"
}
