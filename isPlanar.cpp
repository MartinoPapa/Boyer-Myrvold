#include <iostream>
#include <fstream> // file di testo
#include <vector>
#include "Boyer-Myrvold.h"

using namespace std;

ifstream in("input.txt");
ofstream out("output.txt");

static void readEdges(Graph *g)
{
    int a, b;
    for (size_t i = 0; i < g->m; i++)
    {
        in >> a;
        in >> b;
        g->addAdge_undirected(a, b);
    }
}

int main()
{
    int n, m;
    in >> n;
    in >> m;
    Graph g(n, m);
    readEdges(&g);
    g.DfsTrasversal();
    cout << "separatedDfsChildList di ogni vertice:" << endl;
    for (auto v : g.V)
    {
        cout << v.key << ": ";
        if (v.separatedDfsChildList.count > 0)
        {
            List_Node<Vertex *> *tmp = v.separatedDfsChildList.head;
            do
            {
                cout << "(" << tmp->data->key << "," << tmp->data->lowpoint << "), ";
                tmp = tmp->next;
            } while (tmp != v.separatedDfsChildList.head);
        }
        cout << endl;
    }
    return 0;
}