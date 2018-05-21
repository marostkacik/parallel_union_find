#include <vector>
#include <thread>
#include <cassert>

#include "union_find/lockfree/union_find.hpp"
#include "graph_node/graph_node.hpp"

using namespace std;
using namespace parallel_union_find::union_find::lockfree;
using namespace parallel_union_find::graph_node;

using node = graph_node<union_find>;

vector<node> nodes(2);

int main()
{
    // set labels
    for (int i = 0; i < 2; ++i)
        nodes.at(i).set_label(i);

    // set edges 0 -> 1, 1 -> 0
    nodes.at(0).add_son(&nodes.at(1));
    nodes.at(1).add_son(&nodes.at(0));


    std::thread t1([](){
        while (!nodes.at(0).union_set(&nodes.at(1)));
        assert(nodes.at(1).same_set(&nodes.at(0)));
    });
    std::thread t2([](){
        while (!nodes.at(1).union_set(&nodes.at(0)));
        assert(nodes.at(0).same_set(&nodes.at(1)));
    });

    t1.join();
    t2.join();

    assert(nodes.at(1).same_set(&nodes.at(0)));
    assert(nodes.at(0).same_set(&nodes.at(1)));
}

