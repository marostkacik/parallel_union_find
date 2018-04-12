#pragma once

#include <bits/stdc++.h>

namespace parallel_union_find::graph_node
{
template<typename union_node>
class simple_graph_node;

template<typename union_node>
class simple_graph_node_iterator;

template<typename union_node>
class simple_graph_node : public union_node
{
public:
    using iterator = simple_graph_node_iterator<union_node>;
    friend iterator;

public:
    simple_graph_node();

    // required methods for algorithm
    static std::pair<iterator, iterator>  get_random_neighbors_iterators(simple_graph_node<union_node> const *);

    // helper methods
    static void                           add_son(simple_graph_node<union_node>*, simple_graph_node<union_node>*);
    static void                           set_label(simple_graph_node<union_node>*, size_t);
    static size_t                         get_label(simple_graph_node<union_node> const *);

private:
    mutable std::atomic<size_t>                 _state;
    std::vector<simple_graph_node<union_node>*> _neighbors;
    size_t                                      _label;
};

template<typename union_node>
class simple_graph_node_iterator
{
public:
    simple_graph_node_iterator(simple_graph_node<union_node> const * sgn, size_t steps);

    bool                           operator==(const simple_graph_node_iterator<union_node>& other) const;
    simple_graph_node_iterator&    operator++();
    simple_graph_node<union_node>* operator*() const;

private:
    simple_graph_node<union_node> const * _sgn;
    size_t                                _next_pos;
    size_t                                _steps;
};

#include "simple_graph_node.tpp"
}
