#include <iostream>
#include <vector>

using namespace std;
class UFSet{
private:
    int size;
    int *ufset;
    int *rank;
public:
    UFSet(int n): size(n){
        ufset=new int[n];
        rank=new int[n]();
        for(int i=0;i<n;i++) ufset[i]=i;
    }
    UFSet(int n,int* arr):size(n){
        ufset=new int[n];
        rank=new int[n]();
        memcpy(ufset,arr,n);
    }
    void show(){
        for(int i=0;i<size;i++) cout<<rank[i];
    }
    int Find(int x);
    void Union(int x,int y);
};
int UFSet::Find(int x){
    while(ufset[x]!=x){
        x=ufset[x];
    }
    return x;
}
void UFSet::Union(int x,int y){
    int a=Find(x);
    int b=Find(y);
    if(x==y) return;
    if(rank[a]==rank[b]){
        ufset[x]=b;
        rank[b]+=1;
    }
    else{
        rank[a]>rank[b]?ufset[y]=a:ufset[x]=b;
    }
}



class graph{
private:
    int V;
    vector<int> * adj;
    int **mat;
    UFSet *uf;
public :
    graph(int v){
        V=v;
        adj=new vector<int>[V];
        mat=new int*[V];
        for(int i=0;i<V;i++){
            mat[i]=new int[V]();  
        }
        uf=new UFSet(V);
    }
    graph(int **m,int v){
        V=v;
        mat=m;
        adj=new vector<int> [v];
        transMatToAdj();
        uf=new UFSet(V);
        getUFset();
    }
    graph(vector<int> *a,int v){
        V=v;
        adj=a;
        transAdjToMat();
        uf=new UFSet(V);
        getUFset();
    }
    //void addEdge(int x,int y);
    //void add(vertice);
    void showAdj(){
        for(int i=0;i<V;i++){
            cout<<i+1<<"->";
            for(vector<int>::iterator j=adj[i].begin();j!=adj[i].end();j++) cout<<*j+1<<"--";
            cout<<endl;
        }
    }
    void showMat(){
        for(int i=0;i<V;i++){
            for(int j=0;j<11;j++){
                cout<<mat[i][j];
            }
            cout<<endl;
        }
    }
    bool isDirected();
    void transMatToAdj();
    void transAdjToMat();
    void getUFset();
    void DFS();
    void BFS();
    void isCycle();
    void TopoSort();
    void TopoSortKhan();
    void MST_prim();
    void MST_K();
    void multmat(int flag=0);
};
void graph::transMatToAdj(){
    for(int i=0;i<V;i++){
        for(int j=0;j<V;j++){
            if(mat[i][j]) adj[i].push_back(j);
        }
    }
}
void graph::transAdjToMat(){
    for(int i=0;i<V;i++){
        for(int j=0;j<adj[i].size();j++){
            mat[i][adj[i][j]]=1;
        }
    }
}
bool graph::isDirected(){
    for(int i=0;i<V;i++){
        for(int j=i+1;j<V;j++){
            if(mat[i][j]!=mat[j][i]) return true;
        }
    }
    return false;
}
void graph::multmat(int flag){
    int n=flag;
    vector<int> tmp;
    
    //if(flag) return;
    while(!n){
        n=0;
        for(int i=0;i<V;i++){
            for(int j=0;j<V;j++){
                if(i==j) continue;
                tmp.clear();
                for(int k=0;k<V;k++){
                    if(mat[i][k]*mat[k][j]) {
                        mat[i][j]?mat[i][j]=min(mat[i][j],mat[i][k]+mat[k][j]):mat[i][j]=mat[i][k]+mat[k][j];
                        n+=1;
                    }
                }                
            }
        }
    }
}
void graph::getUFset(){
    for(int i=0;i<V;i++){
        for(int j=0;j<adj[i].size();j++){
            uf->Union(i,adj[i][j]);
        }
    }
}
void test_graph(){
        int  **gmat=new int *[11];
    int arr[][11]={{0,1,1,0,1,0,0,0,0,0,0},{1,0,1,1,0,0,0,0,0,0,0},{1,1,0,1,0,0,0,0,0,0,0},
                        {0,1,1,0,1,1,0,0,0,0,0},{1,0,0,1,0,1,0,0,0,0,0},{0,0,0,1,1,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,1,1,0,1,1},
                        {0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,1,0,0}};//indirected;
    int brr[][11]={{0,0,1,1,0,0,0,0,0,0,0},{0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,1,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0},
                    {0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,1,0,0,0,0},{0,0,0,0,0,0,0,1,0,0,0},
                    {0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,0,1,0,0}};//directed
    
    for(int i=0;i<11;i++){
        gmat[i]=arr[i];
    }
    /*for(int i=0;i<11;i++){
        for(int j=0;j<11;j++){
            cout<<gmat[i][j];
        }
        cout<<endl;
    }*/
    graph *g=new graph(gmat,11);
    g->showMat();
    g->multmat();
    cout<<"----------------------------after------------------------"<<endl;
    g->showMat();    
}
void test_ufset(){
    UFSet *uf=new UFSet(20);
    uf->show();
}
int main(){
    test_ufset();

}