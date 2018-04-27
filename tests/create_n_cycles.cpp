#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

void create_cycle(int start, int many)
{
    for (int i = 0; i < many; ++i)
        cout << start + i << " " << start + i + 1 << "\n";
    cout << start + many << " " << start << "\n";
}

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--help")
    {
        cout << "usage: ./create_n_cycles C N > output.txt\n";
        cout << "C - number of cycles\n";
        cout << "N - number of cities in each cycle\n";
        return 0;
    }

    int c = atoi(argv[1]);
    int n = atoi(argv[2]);

    for (int i = 0; i < c; ++i)
    {
        cout << "0 " << i * n + 1 << "\n";
        create_cycle(i * n + 1, n - 1);
    }

    return 0;
}
