#include <iostream>
#include "prim.hpp"
#include "funcs.hpp"
#include "kruskal.hpp"
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <queue>
#include <csignal>


using namespace std;

void handleClient_LF(int);

Graph g;
mutex mtx;
mutex graph_mtx;

mutex stage1_mtx;
mutex stage2_mtx;
mutex stage3_mtx;
mutex stage4_mtx;

condition_variable stage1_cv;
condition_variable stage2_cv;
condition_variable stage3_cv;
condition_variable stage4_cv;

queue<function<void()>> stage1_q;
queue<function<void()>> stage2_q;
queue<function<void()>> stage3_q;
queue<function<void()>> stage4_q;

queue<int> clientQueue;
condition_variable cond;

void create_graph(int n,int m,int fd=-1){

    lock_guard<mutex> lock(graph_mtx);

    char buffer[1024];

    for(int k=0;k<m;k++){
    	
    	int i;
        int j;
        int w;
        
    	if(fd==-1){
    	    cout<<"error with client fd\n";
    	    exit(1);
    	}
    	
        else{
            ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    	    buffer[bytesRead] = '\0';
    	    string s = buffer;
    	    size_t space = s.find(' ');
    	    i = s.at(space-1) -'0';
    	    j = s.at(space+1) -'0';
    	    string weight = s.substr(space+3);
    	    w = stoi(weight);

        }
        
        g.addEdge(i,j,w);
    }
}


void start_server(int& serverFd,struct sockaddr_in &address){
    int opt = 1;
    int addrlen = sizeof(address);
    // create socket file descriptor
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }

    // set socket options
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(serverFd);
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9034);

    // bind the socket to the port 9034
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(serverFd);
        exit(1);
    }
    
    

    // listen for incoming connections
    if (listen(serverFd, 3) < 0) {
        perror("listen");
        close(serverFd);
        exit(1);
    }

    std::cout << "Server listening on port 9034" << std::endl;
}


//func to manage leadership
void threadWork(int thread_id) {
    while (true) {
        int clientFd = -1;

        {
            unique_lock<mutex> lock(mtx);

            // wait until notify_all() is called. 
            // all the created threads wait here.
            // the thread that has the lock (only one can have it at a time) will continue if the condition is true
            cond.wait(lock, [] { return !clientQueue.empty(); });

	    //cout<<"thread "<<thread_id<<" became the leader\n";
	    
            // pick up the next client from the queue
            if (!clientQueue.empty()) {
                clientFd = clientQueue.front();
                clientQueue.pop();
            }

        }

        // handle the client connection
        if (clientFd != -1) {
            //cout<<"thread "<<thread_id<<" is not the leader anymore\n";
            handleClient_LF(clientFd);
        }
    }
}

void handleClient_LF(int clientFd){
    char buffer[1024];
    string action;
    vector<vector<int>> mst;
    
    while(true){
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            close(clientFd);
            return;
        }

        buffer[bytesRead] = '\0'; 
        action = buffer;
	cout << "Received action: " << action << endl;
	
	try{
		
		lock_guard<mutex> lock(mtx);
		if(action.find("Newgraph")==0){
		    
		    size_t comma = action.find(',');
		    cout<<"Newgraph command:\n";
		    g.getGraph().clear();
		    int n = action.at(comma-1)-'0';
		    int m = action.at(comma+1)-'0';
		    Graph newGraph(n);
		    g = newGraph;
		    
		    create_graph(n,m,clientFd);
		    
		    cout << "Newgraph with " << n << " vertices and " << m << " edges" << endl;
		}
		else if(action.find("prim")==0 || action.find("kruskal")==0){
		    if(g.getGraph().empty()){
		        cout<<"the graph is empty"<<endl;
		    }
		    else{
		    	if(action.find("prim")==0){
		    		mst = calculatePrimMST(g);
		    		cout<<"MST calculated using prim algorithm:"<<endl;
		    	}
		    	else{
		    		mst = calculateKruskalMST(g);
		    		cout<<"MST calculated using kruskal algorithm:"<<endl;
		    	}
		    	
		    	
		    	size_t comma = action.find(',');
		
		    	int v1 = action.at(comma-1)-'0';
		    	int v2 = action.at(comma+1)-'0';
		    	
			calculateTotalWeight(cref(mst), clientFd);
		        calculateLongestDistance(cref(mst), clientFd);
		        calculateAverageDistance(cref(mst), clientFd);
		        calculateShortestDistance(cref(mst),v1,v2,clientFd);
		        
		    }
		}
		
		
		else if(action.find("Newedge")==0){
		    
		    cout<<"Newedge command:\n";
		    size_t comma = action.find(',');
		    
		    int i = action.at(comma-1)-'0';
		    int j = action.at(comma+1)-'0';
		    string weight = action.substr(comma+3);
    	    	    int w = stoi(weight);
		    
		    g.addEdge(i,j,w);
		}
		
		else if(action.find("Removeedge")==0){
		   
		    
		    cout<<"Removeedge command:\n";
		    size_t comma = action.find(',');
		    int i = action.at(comma-1)-'0';
		    int j = action.at(comma+1)-'0';
		    
		    
		    g.removeEdge(i,j);
		}
		
		else{
		    cout<<"no such command\n";
		}
	}
	catch(const std::exception& e){
	    cout<<e.what()<<endl;
	}
        
    }
        
    
}

class Stages{
	public:
	
	vector<vector<int>> mst;
	int clientFd;
	int x;
	int y;

	
	Stages(vector<vector<int>> mst,int clientFd,int x,int y): mst(mst),clientFd(clientFd),x(x),y(y) {}
	
	void stage1(){
	    unique_lock<mutex> lock(stage1_mtx);
	    stage1_cv.wait(lock, [] {return !stage1_q.empty(); });
	    
	    function<void()> task1 = stage1_q.front();
	    
	    stage1_q.pop();
	    
	    stage2_q.push([this] {calculateLongestDistance(this->mst, this->clientFd);} );
	    
	    task1();
	    
	    stage2_cv.notify_one();
	}

	void stage2(){
	    unique_lock<mutex> lock(stage2_mtx);
	    stage2_cv.wait(lock, [] {return !stage2_q.empty(); });
	    
	    function<void()> task2 = stage2_q.front();
	    
	    stage2_q.pop();
	    
	    stage3_q.push([this] {calculateAverageDistance(this->mst, this->clientFd);} );
	    
	    task2();
	    
	    stage3_cv.notify_one();
	}

	void stage3(){
	    unique_lock<mutex> lock(stage3_mtx);
	    stage3_cv.wait(lock, [] {return !stage3_q.empty(); });
	    
	    function<void()> task3 = stage3_q.front();
	    
	    stage3_q.pop();
	    
	    stage4_q.push([this] {calculateShortestDistance(this->mst, this->x, this->y, this->clientFd);} );
	    
	    task3();
	    
	    stage4_cv.notify_one();
	}

	void stage4(){
	    
	    unique_lock<mutex> lock(stage4_mtx);
	    stage4_cv.wait(lock, [] {return !stage4_q.empty(); });
	    
	    function<void()> task4 = stage4_q.front();
	    
	    stage4_q.pop();
	    
	    task4();

	    
 
	}

};


void* handleClient_pipeline(int clientFd){
    char buffer[1024];
    string action;
    vector<vector<int>> mst;
    
    while(true){
        ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            close(clientFd);
            return nullptr;
        }

        buffer[bytesRead] = '\0'; 
        action = buffer;
	cout << "Received action: " << action << endl;
	
	try{
		
		lock_guard<mutex> lock(mtx);
		if(action.find("Newgraph")==0){
		    
		    size_t comma = action.find(',');
		    cout<<"Newgraph command:\n";
		    g.getGraph().clear();
		    int n = action.at(comma-1)-'0';
		    int m = action.at(comma+1)-'0';
		    Graph newGraph(n);
		    g = newGraph;
		    
		    create_graph(n,m,clientFd);
		    
		    cout << "Newgraph with " << n << " vertices and " << m << " edges" << endl;
		}
		else if(action.find("prim")==0 || action.find("kruskal")==0){
		    if(g.getGraph().empty()){
		    	cout<<"the graph is empty"<<endl;
		    }
		    else{
		    	if(action.find("prim")==0){
		    		mst = calculatePrimMST(g);
		    		cout<<"MST calculated using prim algorithm:"<<endl;
		    	}
		    	else{
		    		mst = calculateKruskalMST(g);
		    		cout<<"MST calculated using kruskal algorithm:"<<endl;
		    	}
		    	size_t comma = action.find(',');
		    	int v1 = action.at(comma-1)-'0';
		    	int v2 = action.at(comma+1)-'0';
		    	
		    	// creating threads to perform the calculations
			//thread t1(calculateTotalWeight, cref(mst), clientFd);
		        //thread t2(calculateLongestDistance, cref(mst), clientFd);
		        //thread t3(calculateAverageDistance, cref(mst), clientFd);
		        //thread t4(calculateShortestDistance, cref(mst), v1, v2, clientFd);
		        
		        auto stages = make_shared<Stages>(mst, clientFd, v1, v2); //shared pointer to the object
		        

                    	// Start threads for each stage
                    	thread t1(&Stages::stage1, stages);
                    	thread t2(&Stages::stage2, stages);
                   	thread t3(&Stages::stage3, stages);
                   	thread t4(&Stages::stage4, stages);        
                   	
                   	//pushing the first task to begin the pipeline calculation 
		        stage1_q.push([stages] () {calculateTotalWeight(stages->mst, stages->clientFd);} );
		        stage1_cv.notify_one();
		        
		        t1.join();
			t2.join();
			t3.join();
			t4.join();
		        
		    }
		}
		
		
		else if(action.find("Newedge")==0){
		    
		    cout<<"Newedge command:\n";
		    size_t comma = action.find(',');
		    
		    int i = action.at(comma-1)-'0';
		    int j = action.at(comma+1)-'0';
		    string weight = action.substr(comma+3);
    	    	    int w = stoi(weight);
		    
		    g.addEdge(i,j,w);
		}
		else if(action.find("Removeedge")==0){
		   
		    
		    cout<<"Removeedge command:\n";
		    size_t comma = action.find(',');
		    int i = action.at(comma-1)-'0';
		    int j = action.at(comma+1)-'0';
		    
		    
		    g.removeEdge(i,j);
		}
		
		else{
		    cout<<"no such command\n";
		}
	}
	catch(const std::exception& e){
	    cout<<e.what()<<endl;
	}
        
    }
        
    return nullptr;
    
}

void signalHandler(int signum) { 
    exit(signum);
}



int main(){

    signal(SIGINT, signalHandler);
    
    int serverFd = -1;
    struct sockaddr_in address;
    bool lf = false;
    
    vector<thread> threadpool;
    
    
    
    
    
    if(lf){
        cout<<"server uses the LF pattern\n";
        int numThreads = 4;
        
        for (int i = 0; i < numThreads; ++i) {
            // create a new thread and assign it to threadWork
            threadpool.emplace_back(threadWork,i+1);  
        }
    }
        
    else cout<<"server uses the pipeline pattern\n";
    
    
    start_server(serverFd, address);
    
    while (true) {
        socklen_t addrlen = sizeof(address);
        int clientFd = accept(serverFd, (struct sockaddr*)&address, &addrlen);
        if (clientFd >= 0) {
            cout << "New connection accepted" << endl;
            if(!lf)
            	thread(handleClient_pipeline, clientFd).detach();
            else{
            	clientQueue.push(clientFd);
            	cond.notify_all();
            }
            	
        } 
        else {
            perror("accept");
        }
    }
    

}


