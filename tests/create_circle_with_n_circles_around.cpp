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

int main(int arc, char* argv[])
{
    if (arc == 2 && string(argv[1]) == "--help")
    {
        cout << "usage: ./create_circle_with_n_circles_around N C D\n";
        cout << "N - number of circles on circumference\n";
        cout << "S - size of circle on circumference\n";
        cout << "D - number of nodes between 2 circles on circumference\n";

        return 0;
    }

    int n = atoi(argv[1]);
    int c = atoi(argv[2]);
    int d = atoi(argv[3]);

    int act = 0;

    for (int i = 0; i < n; ++i)
    {
        create_cycle(act, c - 1);

        cout << act << ' ' << act + c << '\n';
        act += c;

        for (int i = 0; i < d; ++i)
            cout << act << ' ' << act + 1 << '\n',
            act++;
    }

    cout << act << ' ' << 0 << endl;

}
