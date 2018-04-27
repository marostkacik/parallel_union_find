#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc == 2 && string(argv[1]) == "--help")
    {
        cout << "usage: ./create_tree N > output.txt\n";
        cout << "N - number of nodes\n";
        return 0;
    }

    int n = atoi(argv[1]);
    mt19937 mt_rand(time(0));

    for (int act = 1; act <= n; ++act)
        cout << mt_rand() % act << ' ' << act << '\n';


    return 0;
}
