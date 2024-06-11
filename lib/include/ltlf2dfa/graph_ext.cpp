#include "ltlf2dfa/graph_ext.h"

typedef unsigned long long ull;

void printGraph(Syn_Graph &graph)
{
    cout << "Graph vertices: ";
    for (auto v : graph.vertices)
        cout << ull(v) << ", ";
    cout << endl;
    cout << "Graph edges: " << endl;
    for (auto it : graph.edges)
    {
        cout << ull(it.first) << ": ";
        for (auto edge : it.second)
            cout << "(" << edge.label->to_string() << ", " << ull(edge.dest) << "), ";
        cout << endl;
    }
}