#include <iostream>
using namespace std;
class LUP{
private:
    int size;
    bool flagLUP;
    bool flagSolve;
    double **A,**U,**L,*P,**Ar;
    void calculate(){   //LU without space optimization;
        for(int i=0;i<size;i++){
            U[i][i]=A[i][i];
            L[i][i]=1;
            for(int j=i+1;j<size;j++){
                L[j][i]=A[j][i]/A[i][i];
                U[i][j]=A[i][j];
            }
            for(int j=i+1;j<size;j++) for(int k=i+1;k<size;k++) A[j][k]=A[j][k]-L[j][i]*U[i][k];
        }
        flagLUP=true;
    }
    void calculate2(){  //LU with space optimization;
        for(int i=0;i<size;i++){
            for(int j=i+1;j<size;j++){
                A[j][i]/=A[i][i];
                for(int k=i+1;k<size;k++) A[j][k]=A[j][k]-A[j][i]*A[i][k];
            }
        }
        flagLUP=true;
    }
    void calculate3(){  //LUP with space optimization;
        for(int i=0;i<size;i++){
            int max=i;
            for(int k=i+1;k<size;k++) if(abs(A[k][i])>abs(A[max][i])) max=k; 
            double *tmp=A[max],swa=0;
            A[max]=A[i];
            A[i]=tmp;
            swa=P[max];
            P[max]=P[i];
            P[i]=swa;
            for(int j=i+1;j<size;j++){
                A[j][i]/=A[i][i];
                for(int k=i+1;k<size;k++) A[j][k]=A[j][k]-A[j][i]*A[i][k];
            }
        }
        flagLUP=true;
    }
    void solve(int *b){
        for(int i=0;i<size;i++) for(int j=0;j<i;j++) b[i]-=A[i][j]*b[j]; //convert b into y;
        for(int i=size-1;i>=0;i--){                                      //convert y into x;
            for(int j=i+1;j<size;j++) b[i]-=b[j]*A[i][j];
            b[i]/=A[i][i];
        }
        flagSolve=true;
    }
    void getReverse(){

    }
public:
    LUP(int n,double** mat):A(mat),size(n){
        U=new double*[n];
        L=new double*[n];
        P=new double[n];
        for(int i=0;i<n;i++){
            U[i]=new double[n]();
            L[i]=new double[n]();
            P[i]=i;            
        }
        flagLUP=false;
    }
    double **getAP(){  //last row of ans is P;
        if(!flagLUP) calculate3();
        double** ans=new double*[size+1];
        for(int i=0;i<size;i++) ans[i]=A[i];
        ans[size]=P;
        return ans;
    }
    
    void showb(int *b){
        if(!flagSolve) solve(b);
        for(int i=0;i<size;i++) cout<<b[i]<<" ";
    }
    void show2(){
        if(!flagLUP) calculate3();
        cout<<'P'<<":"<<endl;
        for(int i=0;i<size;i++)cout<<P[i]<<" ";
        cout<<'A'<<":"<<endl;
        for(int i=0;i<size;i++){
            for(int j=0;j<size;j++) cout<<A[i][j]<<" ";
            cout<<endl;
        }
    }
};
class SolveEquation{
private:
    double** A,*b;
    int size;   
    bool flag;
    void solve(){
        for(int i=0;i<size;i++) for(int j=0;j<i;j++) b[i]-=A[i][j]*b[j]; //convert b into y;
        for(int i=size-1;i>=0;i--){                                      //convert y into x;
            for(int j=i+1;j<size;j++) b[i]-=b[j]*A[i][j];
            b[i]/=A[i][i];
        }
        flag=true;
    }
public:
    SolveEquation(double **mat,double* vec,int n):size(n),flag(false){
        b=new double[n];
        A=(new LUP(n,mat))->getAP();
        for(int i=0;i<size;i++) b[i]=vec[(int)(A[size][i])];
    }
    void show(){
        if(!flag) solve();
        for(int i=0;i<size;i++) cout<<b[i]<<" ";
    }
};

void testLUP(){
    double A[][4]={{2,0,2,0.6},{3,3,4,-2},{5,5,4,2},{-1,-2,3.4,-1}};
    double **p=new double*[4];
    for(int i=0;i<4;i++){
        p[i]=new double[4]();
        for(int j=0;j<4;j++) p[i][j]=A[i][j];
    }
    LUP *lup=new LUP(4,p);
    lup->show2();
}
void testSolveEquation(){      //answer is -3/19,-1/19,59/19;
    double A[3][3]={{1,5,4},{2,0,3},{5,8,2}};
    double b[3]={12,9,5};
    double **p=new double*[3];
    for(int i=0;i<3;i++){
        p[i]=new double[3]();
        for(int j=0;j<3;j++) p[i][j]=A[i][j];
    }
    SolveEquation *se=new SolveEquation(p,b,3);
    se->show();
}
int main(){
    testSolveEquation();
}
