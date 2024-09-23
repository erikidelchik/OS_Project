#include "Graph.hpp"
#include <climits>

int minKey(int key[], bool mstSet[], int V)
{
    // Initialize min value
    int min = INT_MAX, min_index;

    for (int v = 0; v < V; v++)
        if (mstSet[v] == false && key[v] < min){
            min = key[v];
            min_index = v;
        }

    return min_index;
}

vector<vector<int>> calculatePrimMST(Graph &g) {

    vector<vector<int>> graph = g.getGraph();
    int V = graph.size();

    // Array to store constructed MST
    // for example if parent[2] = 1, so vertex 2 is 1's son
    int parent[V];

    // Key values used to pick minimum weight edge in cut
    int key[V];

    // To represent set of vertices included in MST
    bool mstSet[V];

    // Initialize all keys as INFINITE
    for (int i = 0; i < V; i++)
        key[i] = INT_MAX, mstSet[i] = false;

    key[0] = 0;

    // vertex 0 is a root
    parent[0] = -1;

    // The MST will have V vertices
    for (int count = 0; count < V - 1; count++) {

        // Pick the minimum key vertex from the
        // set of vertices not yet included in MST
        int u = minKey(key, mstSet, V);

        // Add the picked vertex to the MST Set
        mstSet[u] = true;

        //update keys
        for (int v = 0; v < V; v++)
            if (graph[u][v] && mstSet[v] == false && graph[u][v] < key[v]){
                parent[v] = u;
                key[v] = graph[u][v];
            }
    }

    vector<vector<int>> mstGraph(V,vector<int>(V,0));
    for(int i=1;i<V;i++){
        int w = graph[parent[i]][i];
        mstGraph[parent[i]][i] = w;
        mstGraph[i][parent[i]] = w;
    }

    return mstGraph;
}


