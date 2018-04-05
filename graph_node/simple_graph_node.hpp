#include <bits/stdc++.h>

namespace parallel_union_find::graph_node
{
template<typename node>
class simple_graph_node_iterator;

template<typename node>
class simple_graph_node;

template<typename node>
class simple_graph_node_iterator
{
public:
    simple_graph_node_iterator<node>& operator++();
    simple_graph_node_iterator<node>  operator++(int);
    bool                              operator==(const simple_graph_node_iterator<node>&);
    bool                              operator!=(const simple_graph_node_iterator<node>&);
    simple_graph_node<node>*          operator*();

private:
    friend simple_graph_node_iterator<node>;

private:
    simple_graph_node<node>* _simple_graph_node;
    size_t                   _act_pos;
    size_t                   _steps_done;
};

template<typename node>
class simple_graph_node
{
public:
    using iterator = typename simple_graph_node_iterator<node>;

public:
    simple_graph_node(node*);

    // required api for algorithm
    std::pair<iterator, iterator> get_random_neighbors_iterators();
    node*                         get_node();

    // api for easier usage
    void add_son(simple_graph_node<node>*);

private:
    friend simple_graph_node_iterator<node>;

private:
    node* const                           _node;
    std::atomic<uint8_t>                  _next_state;
    std::vector<simple_graph_node<node>*> _neighbors;
};

#include "simple_graph_node.tpp"
}
