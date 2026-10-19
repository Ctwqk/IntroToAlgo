#include <iostream>
#include <vector>
#include <queue>

using namespace std;
template<typename T>
class Node{
private:
    T val;
    int num;

public:
    Node(T v,int n):val(v),num(n){}
    void setNum(int n) num=n;
    int getNum() return num;
    void setVal(T v) val=v;
    T getVal() return val; 
};
template<typename T>
class Edge{
private:
    int capacity;
    Node<T> nodeX,nodeY;
public:
    Edge(Node<T>nx,Node<T>ny,int cap):nodeX(nx),nodeY(ny),capacity(cap){}
    pair<int,int>getInt(){
        return make_pair(nodeX.getNum(),nodeY.getNum());
    }
    pair<T,T>getObject(){
        return make_pair(nodeX.getVal(),nodeY.getVal());
    }
    pair<Node<T>,Node<T>>getNode(){return make_pair(nodeX,nodeY)};
};
template<typename T>
class Graph{
private:
    map<T,int> members;
    vector<vector<Edge<T>>> edges;
    vector<Node<T>> vertices;
    vector<vector<int>> mat;
public:
    Graph(int v,vector<T> vertices){
        vector<int> tmp(n,0);
        vertices=tmp;
        vector<Edge<T>> temp;
        for(int i=0;i<v;i++){
            mat.push_back(tmp);
            edges.push_back(temp);
        }
    }
    int addVertice(Node<T> n){
        int V=vertices.size();
        if(members.find(n.getVal())!=members.end())return -1;
        members[n.getVal()]=V;
        for(int i=0;i<V;i++){
            mat.push_back(0);
        }
        mat.push_back(new vector<int>(V+1,0));
        edges.push_back(new vector<Edge<T>>);
        return V;
    }
    bool addEdge(int x,int y,int cap){
        if(x>=members.size()||y>=members.size()) return false;
        mat[x][y]=cap;
        edges[x].push_back(make_pair(vertices[x],vertices[y]));
    }
    
    vector<pair<T,T>> getEdges(){
        vector<pair<T,T>> ans;
        for(int i=0;i<adj.size();i++){
            if(adj[i].size()==1) continue;
            pair<T,T> tmp;
            tmp.first=adj[i][0];
            for(int j=1;j<adj[i].size();j++){
                ans.push_back(tmp);
                ans.back().second=adj[j][0];
            }
        }
        return ans;
    }
    
};

template<typename T>
class FlowGraph{
private:
    Graph<T> capaGraph;
    private Graph<T> resiGraph;

public:
    FlowGraph(vector<Node<T>> vertices){
        capaGraph=new Graph<T>(vertives);
        resiGraph=new Graph<T>(vertices);
    }
    
    void renewFlow(int x,int y,int f){
        
    }


};