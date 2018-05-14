#include <vector>
#include <thread>
#include <iostream>
#include <tuple>
#include "union_node/lockfree/on_the_fly_scc_union_node.hpp"
#include "graph_node/concurrent_graph_node.hpp"
#include "algorithm/concurrent_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_node::lockfree;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;

using node = concurrent_graph_node<on_the_fly_scc_union_node>;

vector<node> nodes(4);

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

    // run
    const int num_threads = 2;
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i)
        threads.emplace_back(concurrent_algorithm<node>, &nodes.at(0), (1 << i));
    for (thread& t : threads)
        t.join();

    for (int i = 0; i < nodes.size(); ++i)
        for (int j = 0; j < nodes.size(); ++j)
        {
            std::pair<bool, bool> ans = nodes.at(i).same_set(&nodes.at(j));

            while (!ans.second)
                ans = nodes.at(i).same_set(&nodes.at(j));

            assert(ans.first);
        }
}

