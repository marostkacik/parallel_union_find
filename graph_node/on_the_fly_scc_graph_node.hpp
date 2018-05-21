#pragma once

#include <vector>
#include <tuple>

namespace parallel_union_find::graph_node
{
    template<typename union_node>
    class on_the_fly_scc_graph_node;

    template<typename union_node>
    class on_the_fly_scc_graph_node_iterator;

    template<typename union_node>
    class on_the_fly_scc_graph_node : public union_node
    {
    public:
        using iterator = on_the_fly_scc_graph_node_iterator<union_node>;
        friend iterator;

    public:
        on_the_fly_scc_graph_node() = default;

        // static cast union_node's methods which return pointer
        on_the_fly_scc_graph_node<union_node>* find_set() const;
        on_the_fly_scc_graph_node<union_node>* get_node_from_set() const;

        // required methods for algorithm
        std::pair<iterator, iterator>          get_random_neighbors_iterators();

        // helper methods
        void                                   add_son(on_the_fly_scc_graph_node<union_node>*);
        void                                   set_label(size_t label);
        size_t                                 get_label() const;

    private:
        std::vector<on_the_fly_scc_graph_node<union_node>*> _neighbors;
        size_t                                              _label;

    };

    template<typename union_node>
    class on_the_fly_scc_graph_node_iterator
    {
    public:
        on_the_fly_scc_graph_node_iterator(const std::vector<on_the_fly_scc_graph_node<union_node>*>& vector, size_t steps);

        bool                                   operator==(const on_the_fly_scc_graph_node_iterator<union_node>& other) const;
        on_the_fly_scc_graph_node_iterator&    operator++();
        on_the_fly_scc_graph_node<union_node>* operator*() const;

    private:
        const std::vector<on_the_fly_scc_graph_node<union_node>*>& _vector;
        size_t                                                     _next_pos;
        size_t                                                     _steps;
    };

#include "on_the_fly_scc_graph_node.tpp"
}
