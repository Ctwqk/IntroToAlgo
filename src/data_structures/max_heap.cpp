#include <iostream>
#include <cmath>
#include <vector>

using namespace std;


struct dat{
private: 
    int key;
    int otherdata;
public:
    dat () {}
    dat(int k):key(k){}
    int getKey() {
        return key;
    }
    int getOtherdate(){return otherdata;}
    bool operator==(const dat d){
        return this->key==d.key;
    }
    bool operator<(const dat d){
        return this->key <d.key;
    }
    bool operator>(const dat d){
        return this->key>d.key;
    }
};
class Heap{
private:
    int cap;
    dat *heap;
    int size;
    int search(dat d);
    void addElement(dat d);
public:
    Heap(vector<int> input,int cap=100):cap(cap),size(0){
        heap = new dat[cap];
        for(int i:input){
            *(heap+size++)=i;
        }
    }
    Heap(int cap=100):cap(cap),size(0){
        heap = new dat[cap];
    }
    void heapify(int pos);
    bool empty();
    int sizee();
    void heapify(dat d);
    void buildHeap();
    void insert(dat d);
    void showHeap();
    dat peek();
    void pop();
};

void Heap::pop(){
    *heap=*(heap+--size);
    heapify(0);
}
int Heap::sizee(){return size;}
bool Heap:: empty(){return size==0;}
dat Heap::peek(){if(this->empty()) cout<<"empty heap"; 
    return NULL;
    return heap[0];
}
int Heap::search(dat d){
    for(int i=0;i<size;i++){
        if(*(heap+i)==d) return i;
    }
    cout<<"no such element in heap";
    return -1;
}
void Heap::heapify(dat d){
    heapify(search(d));
}
void Heap::heapify(int pos){
    int q;
    dat tmp;
    if(pos>size) cout<<"out of bound"<<endl;
    while(2*pos+1<size){
        if(2*pos+2>=size) q=2*pos+1;
        else *(heap+2*pos+1)>*(heap+2*pos+2)?q=2*pos+1:q=2*pos+2;
        if(*(heap+q)>*(heap+pos)){
            tmp=*(heap+q);
            *(heap+q)=*(heap+pos);
            *(heap+pos)=tmp;
            pos=q;
        }
        else break;
    }
}
void Heap::buildHeap(){
    for(int i=size/2-1;i>=0;i--){
        heapify(i);
    }
}
void Heap::addElement(dat d){
    /*if(size>=cap){
        dat* tmp=new dat[cap+cap/10];
        memcpy(tmp,heap,cap);
        heap=tmp;
        delete[] tmp;
    }*/
    *(heap+size++)=d;
}

void Heap::insert(dat d){
    addElement(d);
    int pos=size-1;
    dat tmp;
    while(pos){
        if(*(heap+pos)>*(heap+pos/2)){
            tmp=*(heap+pos);
            *(heap+pos)=*(heap+pos/2);
            *(heap+pos/2)=tmp;
            pos/=2;
        }
        else break;
    }
}
void Heap::showHeap(){
    int index=0;
    for(int i=0;i<=floor(log2(size));i++){
        index=pow(2,i)-1;
        for(int k=0;k<pow(2,i)&&index+k<size;k++){
            cout<<(heap+index+k)->getKey()<<" ";
        }
        cout<<endl;
    }
}
int main(){
    vector<int> values{10,33,44,1323,42,99,0,1,4,29,34,63546,478,4452,464,245,243,345,2345,4,5};
    Heap *h=new Heap();
    for(int i:values) h->insert(dat(i));
    //h->pop();
    h->showHeap();
}