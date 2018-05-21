#include <thread>
#include <vector>
#include <cassert>

#include "union_find/blocking/on_the_fly_scc_union_find.hpp"
#include "graph_node/on_the_fly_scc_graph_node.hpp"
#include "algorithm/multi_core_on_the_fly_scc_decomposition_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_find::blocking;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;

using node = on_the_fly_scc_graph_node<on_the_fly_scc_union_find>;

vector<node> nodes(2);

int main()
{
    // set edges 0 -> 1, 1 -> 0
    nodes.at(0).add_son(&nodes.at(1));
    nodes.at(1).add_son(&nodes.at(0));


    std::thread t1([](){
        nodes.at(0).add_mask(1);
        assert(nodes.at(0).has_mask(1));

        while (!nodes.at(0).union_set(&nodes.at(1)));

        assert(nodes.at(1).same_set(&nodes.at(0)));
        assert(nodes.at(0).has_mask(1));
        assert(nodes.at(1).has_mask(1));
    });
    std::thread t2([](){
        nodes.at(0).add_mask(2);
        assert(nodes.at(0).has_mask(2));

        while (!nodes.at(1).union_set(&nodes.at(0)));

        assert(nodes.at(0).same_set(&nodes.at(1)));
        assert(nodes.at(0).has_mask(2));
        assert(nodes.at(1).has_mask(2));
    });

    t1.join();
    t2.join();

    assert(nodes.at(1).same_set(&nodes.at(0)));
    assert(nodes.at(0).same_set(&nodes.at(1)));
}

