#include <iostream>
#include <vector>
using namespace std;

struct ListNode{
    int val;
    ListNode* next;
    ListNode(int va){val=va;next=nullptr;}
    ListNode(int va,ListNode* n){val=va;next=n;}
    ListNode() :val(0),next(nullptr){}
};

class myHash{
private:
    int size;
    int key;
    ListNode **map;  
public:
    myHash(int si,int k):size(si),key(k){
        map=new ListNode*[size];
        for(int i=0;i<size;i++) map[i]=nullptr;
    }
    ~myHash(){delete map;}
    int Hash_func(int key1);
    void insert(int val);
    void delete_val(int val);
    ListNode* search(int val);
    void modify(int tar,int v);
    void sortedInsert(int val,string ord="increasing");
};
int myHash::Hash_func(int key){
    int mod=10;
    return key%mod;
}
void myHash::insert(int val){
    int k=Hash_func(val);
    if(map[k]==nullptr) map[k]=new ListNode(val);
    else{
        ListNode *p=map[k];
        while(p->next) p=p->next;
        p->next=new ListNode(val);
    }
}
void myHash::delete_val(int val){
    int k=Hash_func(val);
    ListNode*p=map[k];
    while(p->next) p=p->next;
    p=nullptr;
}
ListNode* myHash::search(int v){
    ListNode *p=map[Hash_func(v)];
    while(p){
        if(p->val==v)return p;
        p=p->next;
    }
    return nullptr;
}
void myHash::modify(int t,int v){
    ListNode* tmp=search(t);
    if(tmp==nullptr) cout<<"target element not found";
    else tmp->val=v;
}
void myHash::sortedInsert(int v,string ord="increasing"){
    int k=Hash_func(v);
    if(map[v]==nullptr) map[v]=new ListNode(v);
    else{
        
    }
}
int main(){



}