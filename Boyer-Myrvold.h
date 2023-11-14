#include <stack>
#include <vector>
#include <iostream>
#include "circularDoublyList.h"

const int CLEAR = -1;

using namespace std;

class Vertex;

class Vertex
{
private:
public:
    int key;
    vector<Vertex *> adj; // vicini
    vector<Vertex *> incidentBackEdges;
    Vertex *dfsParent;
    bool isDFSRoot = true; // ! ha figli?
    int leastAnchestor;
    int lowpoint;
    int dfsNumber;
    int backedgeFlag = CLEAR;
    vector<int> pertinentRoots;
    CircularLinkedList<Vertex *> separatedDfsChildList; // the separatedDFSChildList of w still contains references the children of w that remain in separate biconnected components from w.
    List_Node<Vertex *> *pNodeInChildListOfParent;      // posizione di questo vertice nella separedDfsChildList del padre
    Vertex(int key)
    {
        this->key = key;
    }

    void addEdge(Vertex *neighbour)
    {
        adj.push_back(neighbour);
    }

    // O(1)
    // rimuove dalla separatedDfsChildList del dfsParent questo vertice
    void removeFromParentSeparatedDfsChildList()
    {
        dfsParent->separatedDfsChildList.removeAtPosition(pNodeInChildListOfParent);
    }
};

class Graph
{
public:
    int n;
    int m;
    vector<Vertex> V;
    vector<Vertex *> reverseDFSOrder;
    vector<pair<Vertex *, Vertex *>> embeddedEdges;
    stack<int> S; // merge stack
    Graph();

    Graph(int n, int m)
    {
        this->m = m;
        this->n = n;
        for (size_t i = 0; i < n; i++)
        {
            V.push_back(Vertex(i));
        }
    }

    void addAdge_undirected(int key1, int key2)
    {
        V[key1].addEdge(&V[key2]);
        V[key2].addEdge(&V[key1]);
    }

private:
    void updateMaxLowPoint(int &maxLowPoint, int lowpoint)
    {
        maxLowPoint = max(maxLowPoint, lowpoint);
    }

    // O(n+range) dove range = maxVal - minVal + 1, siccome lo useremo su lowValue sappiamo range <= n; => O(2n)
    // NB: //il minVal=minLowPoint sarà sempre = 1 (il lowpoint della radice);
    vector<Vertex *> countingSort(int maxVal)
    {
        // vale quindi range = maxVal
        vector<int> count(maxVal, 0); // inizializzo a 0
        vector<Vertex *> output(n);
        for (auto v : V)
            count[v.lowpoint - 1]++; // conto quanti elementi hanno un determinato lowpoint
        for (int i = 1; i < maxVal; i++)
            count[i] += count[i - 1]; // calcolo le somme cumulative

        for (int i = n - 1; i > -1; i--)
        {
            output[count[V[i].lowpoint - 1] - 1] = &V[i];
            count[V[i].lowpoint - 1]--;
        }
        return output;
    }

    void computeSeparatedDfsChildLists(int maxLowPoint)
    {
        vector<Vertex *> sorteByLowPoint = countingSort(maxLowPoint);
        for (auto u : sorteByLowPoint)
        {
            if (!u->isDFSRoot)
            {
                u->pNodeInChildListOfParent = u->dfsParent->separatedDfsChildList.push_back(u);
            }
        }
    }

    void dfs(int i, int time, int &maxLowPoint, bool *mark)
    {
        time++;
        V[i].dfsNumber = time;
        V[i].lowpoint = time;
        mark[i]=true;
        updateMaxLowPoint(maxLowPoint, V[i].lowpoint);
        for (size_t j = 0; j < V[i].adj.capacity(); j++)
        {
            if (V[i].adj[j]->dfsNumber == 0)
            {
                V[i].adj[j]->dfsParent = &V[i];
                V[i].adj[j]->leastAnchestor = V[i].dfsNumber;
                V[i].adj[j]->isDFSRoot = false;
                dfs(V[i].adj[j]->key, time, maxLowPoint,mark);
                V[i].lowpoint = min(V[i].lowpoint, V[V[i].adj[j]->key].lowpoint);
                updateMaxLowPoint(maxLowPoint, V[i].lowpoint);
            }
            else if (V[i].adj[j]->dfsNumber <= V[i].dfsNumber) // ovvero non è un arco in avanti
            {
                V[i].lowpoint = min(V[i].lowpoint, V[V[i].adj[j]->key].dfsNumber); // esamina i back-edge o cross-edge da v a u
                updateMaxLowPoint(maxLowPoint, V[i].lowpoint);
                if(mark[V[i].adj[j]->key]) V[i].adj[j]->incidentBackEdges.push_back(&V[i]); //isBackEdge
            }
        }
        mark[i]=false;
        reverseDFSOrder.push_back(&V[i]);
    }
    // dimostrazione funzionamento algoritmo per il calcolo dei lowpoints pagina 5 "https://www.cs.cmu.edu/~15451-f17/lectures/lec11-DFS-strong-components.pdf"
    // lowpoint = The lowpoint of a vertex is the DFS ancestor of least DFI that can be reachedby a path of zero or more descendant DFS tree edges plus one back edge  

public:
    // restituisce un vettore di puntatori con i vertici in ordine inverso di dfsNumber (se aciclico reverse topological order) dimostrazione pagina 3 "https://www.cs.cmu.edu/~15451-f17/lectures/lec11-DFS-strong-components.pdf"
    // calcola inoltre lowpoints, dfsNumber, dfsParents e le dfsChildLists
    void DfsTrasversal()
    {
        int maxLowPoint = INT32_MIN;
        bool *mark = new bool[n]; 
        for (size_t i = 0; i < n; i++){
            V[i].dfsNumber = 0;
            mark[i]=false;
        }
            

        for (size_t i = 0; i < n; i++)
        {
            if (V[i].dfsNumber == 0)
                dfs(i, 0, maxLowPoint, mark);
        }
        computeSeparatedDfsChildLists(maxLowPoint);
    }

private:
    static bool pertinent(Vertex *v, Vertex *w)
    {
        return (w->backedgeFlag == v->dfsNumber) || !w->pertinentRoots.empty();
    }

    static bool externallyActive(Vertex *v, Vertex *w)
    {
        bool firstLowpointLessThanV = false;
        if (w->separatedDfsChildList.count > 0)
            firstLowpointLessThanV = w->separatedDfsChildList.head->data->lowpoint < v->dfsNumber;

        return w->leastAnchestor < v->dfsNumber || firstLowpointLessThanV;
    }

    static bool inactive(Vertex *v, Vertex *w)
    {
        return !pertinent(v, w) && !externallyActive(v, w);
    }

    static bool internallyActive(Vertex *v, Vertex *w)
    {
        return pertinent(v, w) && !externallyActive(v, w);
    }

    void embedEdge(Vertex *v, Vertex *w)
    {
        embeddedEdges.push_back(make_pair(v, w));
    }

    void embedTreeEdgesOfDFSChildren(Vertex *v)
    {
        if (v->separatedDfsChildList.count > 0)
        {
            List_Node<Vertex *> *tmp = v->separatedDfsChildList.head;
            do
            {
                embedEdge(v, tmp->data);
                tmp = tmp->next;
            } while (v->separatedDfsChildList.isFirst(tmp));
        }
    }
    void walkup(Vertex *v, Vertex *w){
        //TODO
    }
    void walkdown(Vertex *v, Vertex *w){
        //TODO
    }


public:
    bool isPlanar()
    {
        for (Vertex *v : reverseDFSOrder)
        {
            embedTreeEdgesOfDFSChildren(v);

            for(Vertex* u : v->incidentBackEdges){
                walkup(v,w);
            }

            for(Vertex* u : v->separatedDfsChildList){
                walkdown(v,u);
            }
            
        }
        return true;
    }
};