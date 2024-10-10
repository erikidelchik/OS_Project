#include <iostream>
#include <vector>
#include <climits>
#include <cstring>
#include <unistd.h>
#include <cmath>

using namespace std;

// Helper DFS function to find the path from u to v
bool dfs(const vector<vector<int>>& mst, int u, int v, int& weight, vector<bool>& visited) {

    visited[u] = true;

    // If we reach the destination, return true
    if (u == v) return true;

    // Explore the neighbors
    for (int i = 0; i < mst.size(); i++) {
        if (mst[u][i] && !visited[i]) {
            weight += mst[u][i];
            if (dfs(mst, i, v, weight, visited)) {
                return true;
            }
            // Backtrack if the path doesn't lead to the destination
            weight -= mst[u][i]; 
        }
    }

    visited[u] = false;
    return false;
}

int distance(const vector<vector<int>>& mst, int src, int dest) {
    vector<bool> visited(mst.size(), false);
    int weight = 0;
    
    if (dfs(mst, src, dest, weight, visited)) {
        return weight;
    } 
    else {
        return INT_MAX;
    }
}


void calculateTotalWeight(const vector<vector<int>> &mst,int fd = -1) {
    int weight = 0;
    for(int i=0;i<mst.size();i++){
        for(int j=i;j<mst.size();j++){
            weight+=mst[i][j];
        }
    }
    
    if(fd!=-1){
    	char buffer[1024];
    	string str = "total weight of MST is: "+to_string(weight)+"\n";
    	strcpy(buffer,str.c_str());
	buffer[str.size()]='\0';
	write(fd,buffer,strlen(buffer));
    }
    else
    	cout<<"total weight of MST is: "<<weight<<endl;
    
}

void calculateLongestDistance(const vector<vector<int>> &mst,int fd=-1) {
    int longest = INT_MIN;
    int v1,v2;
    for(int i=0;i<mst.size();i++){
        for(int j=i;j<mst.size();j++){
            int d = distance(mst,i,j);
            if(d>longest && d!=INT_MAX){
                longest = d;
                v1 = i;
                v2 = j;
            }
        }
    }
    if(fd!=-1){
    	char buffer[1024];
    	string str = "longest distance is from "+to_string(v1)+" to "+to_string(v2)+" of weight "+to_string(longest)+"\n";
    	strcpy(buffer,str.c_str());
	buffer[str.size()]='\0';
	write(fd,buffer,strlen(buffer));
    }
    else
    	cout<< "longest distance is from "<<v1<<" to "<<v2<<" of weight "<<longest<<endl;
    
}

void calculateAverageDistance(const vector<vector<int>> &mst,int fd=-1) {
    double pathWeight = 0;
    double numOfPaths = 0;

    for(int i=0;i<mst.size()-1;i++){
        for(int j=i+1;j<mst.size();j++){
            int d = distance(mst,i,j);
            if(d!=INT_MAX){
                pathWeight+=d;
                numOfPaths++;
            }
        }
    }
    if(fd!=-1){
    	char buffer[1024];
    	double avg = pathWeight/numOfPaths;
    	string str = "average distance of all paths in the tree is "+ to_string(avg)+"\n";
    	strcpy(buffer,str.c_str());
	buffer[str.size()]='\0';
	write(fd,buffer,strlen(buffer));
    }
    
    else
    	cout<<"average distance of all paths in the tree is "<<pathWeight/numOfPaths<<endl;
    

}

void calculateShortestDistance(const vector<vector<int>> &mst,int u,int v,int fd=-1) {
    if((u<0 || v<0 || u>=mst.size() || v>= mst.size()) && fd!=-1) {
    	char buffer[1024];
        string str = "invalid verices\n";
        strcpy(buffer,str.c_str());
	buffer[str.size()]='\0';
	write(fd,buffer,strlen(buffer));
        return;
    }
    int d = distance(mst,u,v);
    
    //should not happen because its connected mst
    if(d==INT_MAX && fd!=-1){
    	char buffer[1024];
        string str = "no path between "+to_string(u)+" and "+ to_string(v)+"\n";
        strcpy(buffer,str.c_str());
	buffer[str.size()]='\0';
	write(fd,buffer,strlen(buffer));
        return;
    }
    
    
    if(fd!=-1){
    	char buffer[1024];
    	string str = "shortest path between "+to_string(u)+" and "+to_string(v)+" is of weight "+ to_string(d)+"\n";
    	strcpy(buffer,str.c_str());
	buffer[str.size()]='\0';
	write(fd,buffer,strlen(buffer));
    }
    else
    	cout<<"shortest path between "<<u<<" and "<<v<<" is of weight "<<d<<endl;

}


