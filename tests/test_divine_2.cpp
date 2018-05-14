#include <vector>
#include <thread>
#include <iostream>
#include <cassert>
#include <cstdint>
#include "union_node/lockfree/on_the_fly_scc_union_node.hpp"
#include "graph_node/concurrent_graph_node.hpp"
#include "algorithm/concurrent_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_node::lockfree;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;

using node = concurrent_graph_node<on_the_fly_scc_union_node>;

vector<node> nodes(4);

bool same_set(node* n1, node* n2)
{
    std::pair<bool, bool> ans = n1->same_set(n2);

    while (!ans.second)
        ans = n1->same_set(n2);

    return ans.first;
}

void add_mask(node* n, uint64_t mask)
{
    while (!n->add_mask(mask));
}

int main()
{
    // set labels
    for (int i = 0; i < 4; ++i)
        nodes.at(i).set_label(i);

    // set edges 0 -> 1, 1 -> 2, 2 -> 3, 3 -> 3, 3 -> 0
    nodes.at(0).add_son(&nodes.at(1));
    nodes.at(1).add_son(&nodes.at(2));
    nodes.at(2).add_son(&nodes.at(3));
    nodes.at(3).add_son(&nodes.at(3));
    nodes.at(3).add_son(&nodes.at(0));


    std::thread t1([](){
        while (!nodes.at(0).union_set(&nodes.at(1)));

        assert(same_set(&nodes.at(0), &nodes.at(1)));
        add_mask(&nodes.at(1), 1);

        for (int i = 0; i < 5; ++i)
            if (i == 3)
                nodes.at(1).mark_as_done();
            else
                nodes.at(0).get_node_from_set();

        assert(nodes.at(1).is_done());
    });
    std::thread t2([](){
        while (!nodes.at(0).union_set(&nodes.at(2)));

        assert(same_set(&nodes.at(0), &nodes.at(2)));
        add_mask(&nodes.at(2), 2);

        for (int i = 0; i < 5; ++i)
            if (i == 3)
                nodes.at(2).mark_as_done();
            else
                nodes.at(0).get_node_from_set();

        assert(nodes.at(2).is_done());
    });

    t1.join();
    t2.join();
}

