#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <thread>
#include <chrono>

#include "union_find/lockfree/union_find.hpp"
#include "graph_node/graph_node.hpp"
#include "storage/simple_storage.hpp"

#include "algorithm/parallel_tarjan_algorithm.hpp"
#include "algorithm/sequential_set_based_algorithm.hpp"
#include "algorithm/multi_core_on_the_fly_scc_decomposition_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_find::lockfree;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::algorithm;
using namespace parallel_union_find::storage;

using node = graph_node<union_find>;

std::unordered_map<size_t, size_t> input_to_idx;
simple_storage<node> storage;

void get_input()
{
    size_t from, to;
    size_t read_lines = 0;

    while (cin >> from >> to)
    {
        for (size_t x : {from, to})
            if (input_to_idx.find(x) == input_to_idx.end())
            {
                input_to_idx.emplace(x, input_to_idx.size());

                // make sure we have enough space
                if (storage.capacity() <= input_to_idx.at(x))
                    storage.resize(input_to_idx.at(x) + 1);

                // create node
                new (storage.at(input_to_idx.at(x))) node();

                // give it label from input
                storage.at(input_to_idx.at(x))->set_label(x);
            }

        // add edge from -> to
        storage.at(input_to_idx.at(from))->add_son(storage.at(input_to_idx.at(to)));

        // print progress
        if (++read_lines % 5000000 == 0)
            cerr << "Read " << read_lines / 1000000 << "M rows" << endl;
    }
}

size_t get_first_unused_id()
{
    size_t candidate = 0;

    // first unused id
    while (input_to_idx.find(candidate) != input_to_idx.end())
        ++candidate;

    input_to_idx.emplace(candidate, input_to_idx.size());

    // create node for that id
    if (storage.size() <= input_to_idx.at(candidate))
        storage.resize(input_to_idx.at(candidate) + 1);
    new (storage.at(input_to_idx.at(candidate))) node();

    // connect it to all other nodes, even itself (it does not change anything)
    for (size_t i = 0; i < storage.size(); ++i)
        storage.at(input_to_idx.at(candidate))->add_son(storage.at(i));

    return candidate;
}

bool check_alg(const string& str)
{
    if (str == "TARJAN")
        return true;
    else if (str == "SET_BASED")
        return true;
    else
        return false;
}

int main(int argc, char* argv[])
{
    // help
    if (argc == 2 && string(argv[1]) == "--help")
    {
        cout << "usage: ./test_pokec_simple ALG N < input.txt > ouput.txt\n";
        cout << "ALG - algorithm, <TARJAN, SET_BASED>\n";
        cout << "N - number of threads\n";

        cout << "input format from stdin:\n";
        cout << "each line: number(n1) <delimiter(space, tab)> number(n2)\n";
        cout << "explanation: each line means ordered edge from n1 to n2\n";
        return 0;
    }

    // read input
    const string alg = string(argv[1]);
    const int    threads = atoi(argv[2]);
    size_t       start_node_id;

    if (!check_alg(alg))
    {
        cout << "Don't know " << alg << " algorithm. Use --help option." << endl;
        return 0;
    }
    get_input();
    start_node_id = get_first_unused_id();

    // run algorithm
    auto time_start = chrono::high_resolution_clock::now();

    if (alg == "TARJAN")
    {
        vector<thread> vec_threads;
        node* start_node = storage.at(input_to_idx.at(start_node_id));

        for (int i = 0; i < threads; ++i)
            vec_threads.emplace_back(parallel_tarjan_algorithm<node>, start_node, (1 << i));

        for (int i = 0; i < threads; ++i)
            vec_threads.at(i).join();
    }
    else if (alg == "SET_BASED")
    {
        vector<thread> vec_threads;
        node* start_node = storage.at(input_to_idx.at(start_node_id));

        for (int i = 0; i < threads; ++i)
            vec_threads.emplace_back(sequential_set_based_algorithm<node>, start_node, (1 << i));

        for (int i = 0; i < threads; ++i)
            vec_threads.at(i).join();
    }

    auto time_finish = chrono::high_resolution_clock::now();

    // report information
    cout << "Algorithm finished in " << chrono::duration<double>(time_finish - time_start).count() << " seconds" << endl;

    // number of SCCs
    unordered_set<node*> tops;
    for (const pair<int, int>& p : input_to_idx)
        tops.emplace(storage.at(p.second)->find_set());
    cerr << "Number of components is " << tops.size() << endl;
}

