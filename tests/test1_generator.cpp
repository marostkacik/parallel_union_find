#include <bits/stdc++.h>

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
        cout << "usage: ./test1_generator num_cities > output.txt" << endl;
        return 0;
    }

    int n = atoi(argv[1]);

    for (int c = 0; c < 15; ++c)
    {
        cout << "0 " << c * n + 1 << "\n";
        create_cycle(c * n + 1, n - 1);
    }

    return 0;
}
