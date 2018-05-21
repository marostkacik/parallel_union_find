#pragma once

#include <vector>
#include <tuple>

namespace parallel_union_find::graph_node
{
    template<typename union_node>
    class graph_node;

    template<typename union_node>
    class graph_node_iterator;

    template<typename union_node>
    class graph_node : public union_node
    {
    public:
        using iterator = graph_node_iterator<union_node>;
        friend iterator;

    public:
        graph_node() = default;

        // static cast union_node's method which return pointer
        graph_node<union_node>*        find_set() const;

        // required methods for algorithm
        std::pair<iterator, iterator>  get_random_neighbors_iterators();

        // helper methods
        void                           add_son(graph_node<union_node>*);
        void                           set_label(size_t l);
        size_t                         get_label() const;

    private:
        std::vector<graph_node<union_node>*> _neighbors;
        size_t                               _label;
    };

    template<typename union_node>
    class graph_node_iterator
    {
    public:
        graph_node_iterator(const std::vector<graph_node<union_node>*>& vector, size_t steps);

        bool                    operator==(const graph_node_iterator<union_node>& other) const;
        graph_node_iterator&    operator++();
        graph_node<union_node>* operator*() const;

    private:
        const std::vector<graph_node<union_node>*>& _vector;
        size_t                                      _next_pos;
        size_t                                      _steps;
    };

#include "graph_node.tpp"
}
