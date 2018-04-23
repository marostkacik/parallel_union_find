#include <bits/stdc++.h>
#include "union_node/lockfree/on_the_fly_scc_union_node.hpp"
#include "graph_node/concurrent_graph_node.hpp"
#include "algorithm/concurrent_algorithm.hpp"
#include "storage/simple_storage.hpp"

using namespace std;
using namespace parallel_union_find::union_node::lockfree;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;
using namespace parallel_union_find::storage;

using node = concurrent_graph_node<on_the_fly_scc_union_node>;

int main(int argc, char* argv[])
{
    // help
    if (argc == 2 && string(argv[1]) == "--help")
    {
        cout << "usage: ./test_pokec_concurrent T < input.txt > ouput.txt\n";
        cout << "T - number of threads to use\n";
        return 0;
    }

    // read input
    const int               number_of_threads = atoi(argv[1]);
    const int               start_node = 1;
    int                     from;
    int                     to;
    unordered_map<int, int> seen_ppl; // maps input id to idx
    simple_storage<node>    storage;
    int                     read_rows = 0;

    while (cin >> from >> to)
    {
        // make sure nodes in input are created
        for (int man : {from, to})
            if (seen_ppl.find(man) == seen_ppl.end())
            {
                // mark it as seen
                seen_ppl.emplace(man, seen_ppl.size());

                // make sure storage has allocated enough space
                if (storage.size() <= seen_ppl.at(man))
                    storage.resize(seen_ppl.at(man) + 1);

                // create given node
                new (storage.at(seen_ppl.at(man))) node();

                // give it label as in input
                storage.at(seen_ppl.at(man))->set_label(man);
            }
        // make 'to' neighbor of 'from'
        storage.at(seen_ppl.at(from))->add_son(storage.at(seen_ppl.at(to)));

        // print progress
        if (++read_rows % 5000000 == 0)
            cerr << "Read " << read_rows / 1000000 << "M rows" << endl;
    }

    // start computing
    vector<thread> threads;

    auto time_start = chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_threads; ++i)
        threads.emplace_back(concurrent_algorithm<node>, storage.at(seen_ppl.at(start_node)), (1 << i));
    for (thread& t : threads)
        t.join();
    auto time_finish = chrono::high_resolution_clock::now();

    // print answer
    // cerr << "Algorithm finished in " << chrono::duration<double>(time_finish - time_start).count() << " seconds" << endl;

    // number of SCCs
    unordered_set<node*> tops;
    for (const pair<int, int>& p : seen_ppl)
        tops.emplace(storage.at(p.second)->find_set());
    cerr << "Number of components is " << tops.size() << endl;
}

