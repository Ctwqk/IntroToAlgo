#include <iostream>
#include <vector>
#include <stack>
#include <math.h>
using namespace std;
class lsp{ //not finished; //DP tabulated established, cannot re-establish the optimal resolution;
private:
    int size;
    double **cop;
    double **dptab;
public:
    lsp(int n,double **c):size(n),cop(c){
        dptab=new double*[size];
        for(int i=0;i<n;i++) {
            dptab[i]=new double[size]();
        }
    }
    void solve();
    void solve2();
};
void lsp::solve2(){
    for(int i=0;i<size-1;i++){
        int min=0;
        for(int k=1;k<i;k++){
            if(dptab[min][i]+cop[min][i+1]>dptab[k][i]+cop[k][i+1]) min=k;
        }
        dptab[i][i+1]=dptab[min][i]+cop[min][i+1];
        dptab[i+1][i]=min;
        cop[i+1][i]=1;
        for(int j=i+2;j<size;j++){
            dptab[i][j]=dptab[i][j-1]+cop[j-1][j];
            dptab[j][i]=j-1;
            cop[j][i]=-1;
        }
    }
    dptab[size-1][size-1]=dptab[size-2][size-1]+cop[size-2][size-1];
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++) cout<<dptab[i][j]<<" " ;
        cout<<endl;
    }
    int i=size-2,j=size-1;
    int tmp;
    cout<<j<<"->"<<i<<endl;
    while(i!=0&&j!=0){
        tmp=dptab[j][i];
        if(cop[j][i]=1) cout<<tmp<<"->"<<i<<endl;
        else cout<<i<<"->"<<tmp<<endl;
        if(tmp<i){
            j=i;
            i=tmp;
        }
        else j=tmp;
    }
}
int main(){
    vector<pair<int,int>> cops;
    cops.push_back(make_pair(0,6));
    cops.push_back(make_pair(1,0));
    cops.push_back(make_pair(2,3));
    cops.push_back(make_pair(5,4));
    cops.push_back(make_pair(6,1));
    cops.push_back(make_pair(7,5));
    cops.push_back(make_pair(8,2));
    double **points=new double*[cops.size()];
    for(int i=0;i<cops.size();i++) points[i]=new double[cops.size()]();
    for(int i=0;i<cops.size();i++){
        for(int j=i+1;j<cops.size();j++){
            points[i][j]=sqrt(pow(cops[i].first-cops[j].first,2)+pow(cops[i].second-cops[j].second,2));
        }
    }
    lsp *l=new lsp(cops.size(),points);
    l->solve2();
}