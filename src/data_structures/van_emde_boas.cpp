#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

class vBE{  //not tested, just used to understanding the vBE; only flaw should be within Constructor.
private:
    vBE** cluster;
    vBE* summary;
    int size;
    int min;
    int max;

    int high(int x); 
    int low(int x);
    int idx(int h,int l);
    bool emptyInsert(int x);
public:
    vBE(int s):size(s){

    }
    int maximum();
    int minimum();
    int successor(int x);
    int precessor(int x);
    bool member(int x);
    bool insert(int x);
    bool del(int x);
};
int vBE::high(int x){
    return x/(floor(log2(size))/2);
}
int vBE::low(int x){
    return fmod(x,floor((log2(size))/2));
}
int vBE::idx(int h,int l){
    h*floor(log2(size))/2+l;
}
int vBE::maximum(){
    return max;
}
int vBE::minimum(){
    return min;
}
bool vBE::member(int x){
    if(min==x||max==x) return true;
    else if(size==2) return false;
    else return cluster[high(x)]->member(low(x));
}
int vBE::successor(int x){
    if(size==2){
        if(x==0&&max==1) return 1;
    }
    else if(x<min) return min;
    else if(cluster[high(x)]->maximum()>low(x)) return idx(cluster[high(x)]->successor(low(x)),low(x));
    else if(summary->successor(high(x))>=0)return idx(high(x),cluster[summary->successor(high(x))]->minimum());
    return -1;
}
int vBE::precessor(int x){
    if(size==2){
        if(x==1&&min==0) return 0; 
    }
    else if(x>max) return max;
    else if(cluster[high(x)]->minimum()<low(x)) return idx(cluster[high(x)]->precessor(low(x)),low(x));
    else {
        if(summary->precessor(high(x))>=0) return idx(high(x),cluster[summary->precessor(high(x))]->maximum());
        else return minimum();
    }
    return -1;
}
bool vBE::emptyInsert(int x){
    max=x;
    min=x;
    return true;
}
bool vBE::insert(int x){
    if(minimum()==-1) return(emptyInsert(x));
    if(x<minimum()){
        int tmp=x;
        x=minimum();
        min=tmp;
    }
    if(size>2){
        if(cluster[high(x)]->minimum()==-1) {
            summary->insert(high(x));
            cluster[high(x)]->emptyInsert(low(x));
        }
        else cluster[high(x)]->insert(low(x));
    }
    if(x>max) max=x;
    return true;
}
bool vBE::del(int x){
    if(!member(x)) return false;
    if(min==max) {
        min==-1;
        max==-1;
        return true;
    }
    if(size==2){
        if(x==0) min=1;
        else max=0;
    }
    else{
        if(x==min){
            int tmpMin=summary->minimum();
            x=idx(tmpMin,cluster[tmpMin]->minimum());
            min=x;
        }
        else cluster[high(x)]->del(low(x));
        if(cluster[high(x)]->min==-1){
            summary->del(high(x));
            if(x==max){
                int tmpMax=summary->maximum();
                if(tmpMax==-1) summary->max=summary->min;
                else max=idx(tmpMax,cluster[tmpMax]->maximum());
            }
        }
        else if (x==max){
            max=idx(high(x),cluster[high(x)]->maximum());
        }
        return true;
    }

}

