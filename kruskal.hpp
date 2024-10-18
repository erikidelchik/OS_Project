#include "Graph.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// DSU data structure
// path compression + rank by union
class DSU {
    vector<int> parent;
    vector<int> rank;

public:
    DSU(int n)
    {
        parent.resize(n);
        rank.resize(n);

        for (int i = 0; i < n; i++) {
            parent[i] = -1;
            rank[i] = 1;
        }
    }

    // Find function
    int find(int i)
    {
        if (parent[i] == -1)
            return i;

        return parent[i] = find(parent[i]);
    }

    // Union function
    void unite(int x, int y)
    {
        int s1 = find(x);
        int s2 = find(y);

        if (s1 != s2) {
            if (rank[s1] < rank[s2]) {
                parent[s1] = s2;
            }
            else if (rank[s1] > rank[s2]) {
                parent[s2] = s1;
            }
            else {
                parent[s2] = s1;
                rank[s1] += 1;
            }
        }
    }
};


vector<vector<int>> calculateKruskalMST(Graph &g){
    int size = g.getGraph().size();

    vector<vector<int>> edgelist;
    for(int i=0;i<size;i++){
        for(int j=i;j<size;j++){
            if(g.getGraph()[i][j]){
                edgelist.push_back({g.getGraph()[i][j],i,j});
            }
        }
    }



    vector<vector<int>> mst(size,vector<int>(size,0));

    // Sort all edges
    sort(edgelist.begin(), edgelist.end());

    // Initialize the DSU
    DSU s(size);

    for (auto edge : edgelist) {
        int w = edge[0];
        int x = edge[1];
        int y = edge[2];

        // Take this edge in MST if it does not form a cycle
        if (s.find(x) != s.find(y)) {
            s.unite(x, y);

            mst[x][y] = w;
            mst[y][x] = w;
        }
    }
    return mst;
}


