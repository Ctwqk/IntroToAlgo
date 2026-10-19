#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

class cube{
private:
    int size;
    double ***cb;
    double **xAxisAlpha,**yAxisBeta,**zAxisTheta;
    double **rotateMat;

    void setxAxisAlpha(double alpha);
    void setyAxisBeta(double beta);
    void setzAxisTheta(double theta);
    void setAxisAng(int idx,double ang); //x=0,y=1,z=2;
    void setRotateMat(double ang);
    double** matMult(double **a,double **b,int n,int k,int m);
public:
    static const int pi=M_PI;

    cube(int s):size(s){
        xAxisAlpha=new double*[3];
        yAxisBeta=new double*[3];
        zAxisTheta=new double*[3];
        for(int i=0;i<3;i++){
            xAxisAlpha[i]=new double[3]();
            yAxisBeta[i]=new double[3]();
            zAxisTheta[i]=new double[3]();
        }
        rotateMat=new double*[2]{new double[2],new double[2]};
        cb=new double**[size];
        for(int i=0;i<size;i++){
            cb[i]=new double*[size];
            for(int j=0;j<size;j++){
                cb[i][j]=new double[3]();
                cb[i][j][0]=j-size/2.0;
                cb[i][j][1]=i-size/2.0;
                cb[i][j][2]=size/2.0;

            }
        }
    }
    void getRotateMat(double alpha,double beta,double theta);
    void getRotateMatd(double alphaD,double betaD,double thetaD);
    void rotate();
    void show();
};
void cube::show(){
    char** cbshow=new char*[size];
    for(int i=0;i<size;i++) {
        cbshow[i]=new char[size];

        for(int j=0;j<size;j++){
            cout<<i<<" "<<j<<endl;
            cout<<(cb[i][j][1]+size/2.0)<<' '<<(cb[i][j][0]+size/2.0)<<endl;
            cbshow[(int)round(cb[i][j][1]+size/2.0)][(int)round(cb[i][j][0]+size/2.0)]='*';
            //cout<<(cb[i][j][1])<<" "<<(cb[i][j][0])<<" "<<i<<" "<<j<<endl;
            //cout<<(cb[i][j][1]+size/2.0)<<" "<<(cb[i][j][0]+size/2.0)<<endl;

        }
    }
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            cout<<' '<<cbshow[i][j]<<' ';
        }
        cout<<endl;
    }
}
void cube::rotate(){
    double **rotMat= matMult(matMult(xAxisAlpha,yAxisBeta,3,3,3),zAxisTheta,3,3,3);
    for(int i=0;i<size;i++){
        cb[i]=matMult(cb[i],rotMat,size,3,3);
        //for(int j=0;j<size;j++) cout<<cb[i][j][1]<<endl;
    }
}
double** cube::matMult(double **a, double **b,int n,int k,int m) {
    double **ans=new double*[n];
    for(int i=0;i<n;i++){
        ans[i]=new double[m]();
        for(int j=0;j<m;j++){
            for(int l=0;l<k;l++){
                ans[i][j]+=a[i][l]*b[l][j];
            }
        }
    }
    return ans;
}
void cube::getRotateMatd(double alphaD, double betaD, double thetaD) {
    double alpha=fmod(alphaD,180)*pi/180.0;
    double beta=fmod(betaD,180)*pi/180.0;
    double theta=fmod(thetaD,180)*pi/180.0;
    getRotateMat(alpha,beta,theta);
}
void cube::getRotateMat(double alpha, double beta, double theta) {
    alpha=fmod(alpha,2*pi);
    beta=fmod(beta,2*pi);
    theta=fmod(theta,2*pi);
    setxAxisAlpha(alpha);
    setyAxisBeta(beta);
    setzAxisTheta(theta);

}
void cube::setxAxisAlpha(double alpha) {
    setAxisAng(0,alpha);
}
void cube::setyAxisBeta(double beta) {
    setAxisAng(1,beta);
}
void cube::setzAxisTheta(double theta) {
    setAxisAng(2,theta);
}
void cube::setAxisAng(int idx,double ang) {
    setRotateMat(ang);
    double **AxisMat;
    switch (idx) {
        case 0:
            AxisMat=xAxisAlpha;
            break;
        case 1:
            AxisMat=yAxisBeta;
            break;
        case 2:
            AxisMat=zAxisTheta;
            break;
    }
    for(int i=0;i<3;i++) for(int j=0;j<3;j++) AxisMat[i][j]=0;
    AxisMat[idx][idx]=1;
    for(int i=0;i<2;i++){
        for(int j=0;j<2;j++){
            AxisMat[(idx+i+1)%3][(idx+j+1)%3]=rotateMat[i][j];
        }
    }

}
void cube::setRotateMat(double ang) {
    rotateMat[0][0]=cos(ang);
    rotateMat[0][1]=-sin(ang);
    rotateMat[1][0]=sin(ang);
    rotateMat[1][1]=cos(ang);
}

int main(){
    cube *c=new cube(50);
    c->getRotateMat(M_PI/2,M_PI/6,M_PI/3);
    c->rotate();
    //c->show();
    int a=0;
    //cin>>a;
}