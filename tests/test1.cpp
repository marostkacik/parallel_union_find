// reads connections from stdin in format A B where A and B are ints and mean that there's an edge from A to B. Starts searching from from 0
// on stdout outputs on each line number one scc
// runs on all threads

#include <bits/stdc++.h>
#include "union_node/waitfree/simple_union_node.hpp"
#include "union_node/lockfree/simple_union_node.hpp"
#include "graph_node/simple_graph_node.hpp"
#include "storage/per_thread_reserve_storage.hpp"
#include "storage/per_thread_reserve_storage_accessor.hpp"
#include "algorithm/simple_algorithm.hpp"

using namespace std;
using namespace parallel_union_find::union_node;
using namespace parallel_union_find::graph_node;
using namespace parallel_union_find::storage;
using namespace parallel_union_find::algorithm;

using node = simple_graph_node<lockfree::simple_union_node>;

per_thread_reserve_storage<node> storage;

int main()
{
    int a, b;
    unordered_set<int> seen;

    int lines = 0;
    while (cin >> a >> b)
    {
        for (int n : {a, b})
            if (seen.find(n) == seen.end())
            {
                seen.insert(n);
                if (n >= storage.capacity())
                    storage.resize(n + 1);

                new (storage.at(n)) node();
                storage.at(n)->set_label(n);
            }

        storage.at(a)->add_son(storage.at(b));

        if (++lines % 10000000 == 0)
            std::cerr << "read " << lines / 1000000 << "M lines " << std::endl;
    }

    // print addresses
    // for (int i : seen)
    //     std::cerr << i << " on " << storage.at(i) << std::endl;

    // start scc finding
    vector<thread> threads;

    std::cerr << "loading done" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < thread::hardware_concurrency(); ++i)
        threads.emplace_back(simple_algorithm<node>, storage.at(0));
    for (thread& t : threads)
        t.join();
    auto finish = std::chrono::high_resolution_clock::now();
    std::cerr << "algorithm done in " << std::chrono::duration<double>(finish - start).count() << std::endl;

    // collect data
    unordered_map<node*, vector<int>> um;

    for (int i : seen)
    {
        node* repr = static_cast<node*>(storage.at(i)->find_set());
        um[repr].push_back(i);
    }

    // print data
    for (const pair<node* const, vector<int>>& p : um)
    {
        for (int i : p.second)
            cout << i << " ";
        cout << endl;
    }

}
