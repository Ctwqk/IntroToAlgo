#include <iostream>

using namespace std;
class knapsack{
private:
    int size;
    int capacity;//weight capacity;
    pair<int,int> *value_weight;
    int **dptab;
public:
    knapsack(int n,pair<int,int> *v_w,int cap):size(n),value_weight(v_w),capacity(cap){
        dptab=new int*[size+1];
        for(int i=0;i<=size;i++) dptab[i]=new int[capacity+1]();
    }
    void solve();
    
};
void knapsack::solve(){
    for(int i=1;i<=size;i++){
        for(int j=1;j<=capacity;j++){
            if(value_weight[i-1].second>j) continue;
            dptab[i][j]=max(dptab[i-1][j],dptab[i-1][j-value_weight[i-1].second]+value_weight[i-1].first);
        }
    }
    for(int i=1;i<size;i++) if(dptab[i][capacity]>dptab[size][capacity]) dptab[size][capacity]=dptab[i][capacity];
    cout<<dptab[size][capacity];
}
int main(){
    pair<int,int> *p=new pair<int,int> [3];
    p[0]=make_pair(100,20);
    p[1]=make_pair(120,30);
    p[2]=make_pair(60,10);
    knapsack *k=new knapsack(3,p,50);
    k->solve();
}
