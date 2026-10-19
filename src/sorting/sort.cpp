#include <iostream>
#include <vector>

using namespace std;
struct dat{
    int* key;
    int data,digit;
    dat* next,*pre;
    dat(){
        key=new int(100);
        digit=100;
    }
    dat(int size){
        key=new int(size);
        digit=size;
    }
    dat(vector<int> a){
        digit=a.size();
        key=new int(digit);
        for(int i=0;i<digit;i++){
            *(key+i)=a[i];
        }
    }
    bool comp(dat* d,int i){
        return *(this->key+i)>*(d->key+i);
    }
    void show(){
        for(int i=0;i<digit;i++) cout<<*(key+i)<<" ";
        cout<<endl;
    }
    
};
class Sort
{
private:
    int *arr;
    int size;
    //vector<dat*> dats;
public:
    /*Sort(vector<dat*>das){
        for(dat* da:das) dats.push_back(da);
        size=dats.size();
    }*/
    Sort(){
        size=100;
        arr=new int[size];
    }
    Sort(vector<int> a){
        size=a.size();
        arr=new int[size];
        for(int i=0;i<size;i++){
            arr[i]=a[i];
        }
    }
    void bubble_sort();
    void insert_sort();
    void select_sort();
    void quick_sort(int a,int b);
    int partition_qsa_1(int a,int b);
    int partition_qsa_2(int a,int b);
    void counting_sort();
    void radix_sort(vector<dat*> &t,int digit);
    void aux_counting_radix(vector<dat*> &ds,int digit);
    void checkout(){
        for(int i=0;i<size;i++) cout<<*(arr+i)<<" ";
    }
};
void Sort::bubble_sort(){
    int flag=1,tmp;
    for(int i=1;i<size&&flag==1;i++){
        flag=0;
        for(int j=0;j<size-i;j++){
            if(arr[j]>arr[j+1]){
                tmp=arr[j];
                arr[j]=arr[j+1];
                arr[j+1]=tmp;
                flag=1;
            }
        }
    }
}
void Sort::insert_sort(){
    int tmp,j;
    for(int i=1;i<size;i++){
        tmp=arr[i];
        j=i;
        while(j>0&&tmp<arr[j-1]){
            arr[j]=arr[j-1];
            j--;
        }
        arr[j]=tmp;
    }
}
void Sort::select_sort(){
    int min,tmp;
    for(int i=0;i<size;i++){
        min=i;
        for(int j=i;j<size;j++){
            if(arr[j]<arr[min]) min=j;
        }
        tmp=arr[min];
        arr[min]=arr[i];
        arr[i]=tmp;
    }
}
int Sort::partition_qsa_1(int a,int b){
    if(a>=b) return -1;
    int i=a,j=a,tmp=arr[b],swa;
    while(j<b){
        if(arr[j]<tmp){
            swa=arr[i];
            arr[i]=arr[j];
            arr[j]=swa;
            i++;
        }
        j++;
    }
    swa=arr[b];
    arr[b]=arr[i];
    arr[i]=swa;
    return i;
}
int Sort::partition_qsa_2(int a,int b){
    if(a>=b) return -1;
    int tmp=arr[a],i=a,j=b;
    while(i<j){
        while(j>i&&arr[j]>=tmp) j--;
        arr[i]=arr[j];
        while(i<j&&arr[i]<=tmp) i++;
        arr[j]=arr[i];
    }
    arr[i]=tmp;
    return i;
}
void Sort::quick_sort(int a,int b){
    //int i=partition_qsa_1(a,b);
    int i=partition_qsa_2(a,b);
    if(i==-1) return;
    quick_sort(a,i-1);
    quick_sort(i+1,b);
}
void Sort::counting_sort(){
    int * brr=new int[size+1],min=arr[0],max=arr[0];
    for(int i=1;i<size;i++){
        if(arr[i]>max) max=arr[i];
        if(arr[i]<min) min=arr[i];
    }
    int n=max-min+1;
    int * crr=new int[n];
    for(int i=0;i<n;i++){
        crr[i]=0;
    }
    for(int i=0;i<size;i++){
        crr[arr[i]-min]++;
    }
    for(int i=1;i<n;i++){
        crr[i]+=crr[i-1];
    }
    for(int i=size-1;i>=0;i--){
        brr[crr[arr[i]-min]]=arr[i];
        crr[arr[i]-min]--;
    }
    for(int i=0;i<size;i++) arr[i]=brr[i+1];
}
void Sort::aux_counting_radix(vector<dat*> &ds,int digi){  //sorting base on the digitth dight value;
    int n=ds.size(); 
    dat** brr=new dat*[n];
    dat* min=ds[0],*max=ds[0];
    for(int i=0;i<n;i++){
        if(*(ds[i]->key+digi)>*(max->key+digi)) {
            max=ds[i];
            cout<<1;
        }
        if(*(ds[i]->key+digi)<*(min->key+digi)) {
            min=ds[i];
            cout<<2;
        }
    }
    cout<<*(min->key+digi);
    cout<<*(max->key+digi);
    int m=*(max->key+digi)-*(min->key+digi)+1;
    cout<<m<<"   ";
    int *crr=new int[m];
    for(int i=0;i<m;i++){
        crr[i]=0;
    }
    for(int i=0;i<n;i++){
        crr[*(ds[i]->key+digi)-*(min->key+digi)]++;
    }
    for(int i=1;i<m;i++){
        crr[i]+=crr[i-1];
    }
    for(int i=n-1;i>=0;i--){
        brr[crr[*(ds[i]->key+digi)-*(min->key+digi)]-1]=ds[i];
        crr[*(ds[i]->key+digi)-*(min->key+digi)]--;
    }
    for(int i=0;i<n;i++){
        ds[i]=brr[i];
    }
}
void Sort::radix_sort(vector<dat*> &t,int digit){
    if(!t.size()) return ;
    for(int i=digit-1;i>=0;i--){
        aux_counting_radix(t,i);
    }
}
void test_radix(int size,int digit){
    vector<dat*> try_rsort;
    for(int i=0;i<size;i++){
        dat *tmp=new dat(digit);
        for(int j=0;j<digit;j++){
            *(tmp->key+j)=rand()%10;
        }
        try_rsort.push_back(tmp);
    }
    Sort *rsort=new Sort();
    rsort->radix_sort(try_rsort,digit);
    for(int i=0;i<size;i++){
        try_rsort[i]->show();
    }
}
int main(){
    //vector<int> try_sort={3,5,1,3,5,7,8,3,2,5,10,33,44,1323,42,99,0,1,4,29,4,5};
    //Sort s(try_sort);
    //s.bubble_sort();
    //s.counting_sort();
    //s.checkout();
    test_radix(15,5);
}