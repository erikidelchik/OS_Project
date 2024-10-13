#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>

using namespace std;

int max(int i, int j){
    if(i>j) return i;
    return j;
}

class Graph{
    //directed graph
    vector<vector<int>> adjMat;

public:
    Graph() = default;
    Graph(int size){
        adjMat.resize(size,vector<int>(size));
    }

    vector<vector<int>> getGraph(){
        return adjMat;
    }

    explicit Graph(vector<vector<int>> other){
        adjMat.clear();
        for(int i=0;i<other.size();i++){
            adjMat.push_back(other[i]);
        }
    }
    
    Graph& operator=(Graph &other){
    	adjMat.clear();
    	adjMat = other.getGraph();
    	
    	return *this;
    
    }

    void addEdge(int v1,int v2,int w){
        if(!adjMat[v1][v2]){
            adjMat[v1][v2] = w;
            adjMat[v2][v1] = w;
        }
    }

    void removeEdge(int v1,int v2){
        if(adjMat[v1][v2]){
            adjMat[v1][v2] = 0;
            adjMat[v2][v1] = 0;
        }

    }



};

#endif
