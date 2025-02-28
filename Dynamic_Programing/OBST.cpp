#include <iostream> 
#include <queue>
using namespace std;
class obst{
private:
    int size;
    double *p;
    double *q;
    int **root;
    double **e,**w;
public:
    obst(int n,double *a,double *b):size(n){
        p=new double[size]();
        q=new double[size]();
        for(int i=1;i<size;i++){
            p[i]=a[i-1];
            q[i]=b[i];
        }
        root=new int*[size-1];
        for(int i=0;i<size-1;i++) root[i]=new int[size-1]();
        q[0]=b[0];
        e=new double*[size+1];
        w=new double*[size+1];
        for(int i=1;i<size+1;i++){
            e[i]=new double[size]();
            w[i]=new double[size]();
            e[i][i-1]=q[i-1];
            w[i][i-1]=q[i-1];
        }
    }
    void find_obst2();
    void find_obst();
    void show_obst();
};
void obst::find_obst2(){
    for(int k=0;k<size;k++){
        for(int i=1;i<size-k;i++){
            int j=i+k;
            w[i][j]=p[j]+q[j]+w[i][j-1];
            e[i][j]=e[i][i-1]+e[i+1][j]+w[i][j];
            root[i-1][j-1]=i;
            for(int l=i+1;l<=j;l++){
                if(e[i][l-1]+e[l+1][j]+w[i][j]<e[i][j]){
                    e[i][j]=e[i][l-1]+e[l+1][j]+w[i][j];
                    root[i-1][j-1]=l;
                }
            }
        }
    }
}
void obst::show_obst(){   
    queue<pair<int,int>> bfs; 
    bfs.push(make_pair(0,size-2));
    pair<int,int> tmp;
    int ro,num1=1,num2=0;
    cout<<"root is k" <<root[0][size-2]<<", the OBST expectation is "<<e[1][size-1]<<endl;
    while(bfs.size()){
        tmp=bfs.front();
        bfs.pop();
        ro=root[tmp.first][tmp.second]-1;
        if(tmp.first==tmp.second) {
            cout<<"the left child of k"<<tmp.first+1<<" is d"<<tmp.second<<endl;
            cout<<"the right child of k"<<tmp.first+1<<" is d"<<tmp.second+1<<endl;
        }
        else if(ro==tmp.first){
            cout<<"the left child of k"<<ro+1<<" is d"<<ro<<endl;
            cout<<"the right child of k"<<ro+1<<" is k"<<root[ro+1][tmp.second]<<endl;
            bfs.push(make_pair(ro+1,tmp.second));
        }
        else if(ro==tmp.second){
            cout<<"the right child of k"<<ro+1<<" is d"<<ro+1<<endl;
            cout<<"the left child of k"<<ro+1<<" is k"<<root[tmp.first][ro-1]<<endl;
            bfs.push(make_pair(tmp.first,ro-1));
        }
        else{
            bfs.push(make_pair(tmp.first,ro-1));
            bfs.push(make_pair(ro+1,tmp.second));
            cout<<"the left child of k"<<ro+1<<" is k"<<root[tmp.first][ro-1]<<endl;
            cout<<"the right child of k"<<ro+1<<" is k"<<root[ro+1][tmp.second]<<endl;
        }
    }
}
int main(){
    double q[]={0.06,0.06,0.06,0.06,0.05,0.05,0.05,0.05};
    double p[]={0.04,0.06,0.08,0.02,0.1,0.12,0.14};
    //double q[]={0.05,0.1,0.05,0.05,0.05,0.1};
    //double p[]={0.15,0.1,0.05,0.1,0.2};
    obst *ob=new obst(8,p,q);
    ob->find_obst2();
    ob->show_obst();
}