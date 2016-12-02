#include <cstdlib>
#include "graph.h"

using namespace std;

int main(int argc, char** argv)
{
    C_graph G;

    G.random_graph(10, 0.5, 5, 100);

    G.affiche();

    return 0;
}

