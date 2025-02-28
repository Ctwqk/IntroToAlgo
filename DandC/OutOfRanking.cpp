#include <iostream>
#include <vector>

using namespace std;

class OutOfRanking{
private:   
    int count;
public:
    OutOfRanking():count(0){}
    void user_similarity(vector<int> &RankA);
    void auxilary_merge(vector<int> &nums,int i,int j);
};

void OutOfRanking::user_similarity(vector<int> &RankA){
    int n=RankA.size();
    auxilary_merge(RankA,0,n-1);
    cout<<"similiarity is: "<<count;
}

void OutOfRanking::auxilary_merge(vector<int> &nums,int i,int j){
    if(i>=j) return;
    int mid=(i+j)/2;
    auxilary_merge(nums,i,mid);
    auxilary_merge(nums,mid+1,j);
    int *ext=new int[j-i+1];
    int l=i,r=mid+1,idx=0;
    while(l<=mid&&r<=j){
        while(l<=mid&&nums[l]<nums[r])ext[idx++]=nums[l++];
        count+=mid-l+1;
        ext[idx++]=nums[r++];
    }
    while(l<=mid) ext[idx++]=nums[l++];
    while(r<=j) ext[idx++]=nums[r++];
    for(int k=i;k<=j;k++)nums[k]=ext[k-i];
}

int main(){
    vector<int> RankA{2,4,5,1,3,6,7};
    OutOfRanking *o=new OutOfRanking();
    o->user_similarity(RankA);
}