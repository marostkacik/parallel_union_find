#include <bits/stdc++.h>

namespace parallel_union_find::graph_node
{
template<typename union_node>
class simple_graph_node : public union_node
{
public:
    using iterator = typename std::vector<simple_graph_node<union_node>*>::iterator;

public:
    std::pair<iterator, iterator> get_random_neighbors_iterators();
    void add_son(simple_graph_node<union_node>*);

private:
    std::vector<simple_graph_node<union_node>*> _neighbors;
};

#include "simple_graph_node.tpp"
}
