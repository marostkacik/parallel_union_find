#include <thread>

#include "union_find/blocking/on_the_fly_scc_union_find.hpp"
#include "graph_node/on_the_fly_scc_graph_node.hpp"
#include "algorithm/parallel_tarjan_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_find::blocking;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;

using node = on_the_fly_scc_graph_node<on_the_fly_scc_union_find>;

node n1;
node n2;

int main()
{
    n1.add_son(&n2);
    n2.add_son(&n1);

    // run
    thread t1(parallel_tarjan_algorithm<node>, &n1, 1);
    parallel_tarjan_algorithm<node>(&n1, 2);

    // wait for computation to end
    t1.join();

    // check result
    assert(n1.same_set(&n2));
    assert(n2.same_set(&n1));
}

