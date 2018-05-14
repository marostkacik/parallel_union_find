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

vector<node> nodes(2);

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
    for (int i = 0; i < 2; ++i)
        nodes.at(i).set_label(i);

    // set edges 0 -> 1, 1 -> 2, 2 -> 3, 3 -> 3, 3 -> 0
    nodes.at(0).add_son(&nodes.at(1));
    nodes.at(1).add_son(&nodes.at(0));


    std::thread t1([](){
        add_mask(&nodes.at(0), 1);
        assert(nodes.at(0).has_mask(1));

        while (!nodes.at(0).union_set(&nodes.at(1)));

        assert(same_set(&nodes.at(1), &nodes.at(0)));
        assert(nodes.at(0).has_mask(1));
        assert(nodes.at(1).has_mask(1));
    });
    std::thread t2([](){
        add_mask(&nodes.at(0), 2);
        assert(nodes.at(0).has_mask(2));

        while (!nodes.at(1).union_set(&nodes.at(0)));

        assert(same_set(&nodes.at(0), &nodes.at(1)));
        assert(nodes.at(0).has_mask(2));
        assert(nodes.at(1).has_mask(2));
    });

    t1.join();
    t2.join();

    assert(same_set(&nodes.at(1), &nodes.at(0)));
    assert(same_set(&nodes.at(0), &nodes.at(1)));
}

