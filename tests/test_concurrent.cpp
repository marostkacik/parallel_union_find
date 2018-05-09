#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <thread>
#include <chrono>
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
        cout << "usage: ./test_pokec_simple T < input.txt > ouput.txt\n";
        cout << "T - number of threads to use\n";

        cout << "input format from stdin:\n";
        cout << "each line: number(n1) <delimiter(space, tab) number(n2)>\n";
        cout << "explanation: each line means ordered edge from n1 to n2\n";
        return 0;
    }

    // read input
    const int               number_of_threads = atoi(argv[1]);
    int head_id = 0;
    unordered_map<int, int> input_to_index;
    simple_storage<node>    storage;

    int from, to, read_rows = 0;
    while (cin >> from >> to)
    {
        for (int x : {from, to})
            if (input_to_index.find(x) == input_to_index.end())
            {
                input_to_index.emplace(x, input_to_index.size());

                // make sure storage has allocated enough space
                if (storage.capacity() <= input_to_index.at(x))
                    storage.resize(input_to_index.at(x) + 1);

                // create node
                new (storage.at(input_to_index.at(x))) node();

                // give it label from input
                storage.at(input_to_index.at(x))->set_label(x);
            }

        // make 'to' neighbor of 'from'
        storage.at(input_to_index.at(from))->add_son(storage.at(input_to_index.at(to)));

        // print progress
        if (++read_rows % 5000000 == 0)
            cerr << "Read " << read_rows / 1000000 << "M rows" << endl;
    }

    // find first unused id
    while (input_to_index.find(head_id) != input_to_index.end())
        head_id++;

    // create new head node
    input_to_index.emplace(head_id, input_to_index.size());
    if (storage.capacity() <= input_to_index.at(head_id))
        storage.resize(input_to_index.at(head_id) + 1);

    new (storage.at(input_to_index.at(head_id))) node();

    // connect it to everything
    for (int i = 0; i < storage.size(); ++i)
        storage.at(input_to_index.at(head_id))->add_son(storage.at(i));

    // start computing from head node
    vector<thread> threads;

    auto time_start = chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_threads; ++i)
        threads.emplace_back(concurrent_algorithm<node>, storage.at(input_to_index.at(head_id)), (1 << i));
    for (thread& t : threads)
        t.join();
    auto time_finish = chrono::high_resolution_clock::now();

    // print answer
    cerr << "Algorithm finished in " << chrono::duration<double>(time_finish - time_start).count() << " seconds" << endl;

    // number of SCCs
    unordered_set<node*> tops;
    for (const pair<int, int>& p : input_to_index)
        tops.emplace(storage.at(p.second)->find_set());
    cerr << "Number of components is " << tops.size() << endl;
}

