#include <iostream>
#include <vector>
#include <stack>
#include <queue>
using namespace std;

class numbers{
private:
    vector<vector<int>> ans;
    int layer;
    int n;
    int shd1;
    int shd2;
public:
    numbers(int n,int shd1,int shd2):n(n),shd1(shd1),shd2(shd2),layer(0){
        //shd1: increasing 阈值;  shd2 decreasing 阈值;
    }
    bool judge(vector<int> stage, int idx,vector<pair<int,int>> &id);
    void find();
    void show();
};
void numbers::show(){   //output the answer of question
    cout<<"question: "<<n<<" number "<<shd1<<" increasing and "<<shd2<<" decreasing;"<<endl;
    cout<<"list of 100 possible answers:"<<endl;
    for(int i=0;i<100&&i<ans.size();i++){
        for(int i:ans[i]) cout<<i<<" ";
        cout<<endl;
    }
    cout<<"number of answers: "<<ans.size();
}
void numbers::find(){  //find possible lists;
    queue<pair<vector<int>,vector<pair<int,int>>>> bfs; 
    vector<int> tmp;
    vector<pair<int,int>> check;
    pair<vector<int>,vector<pair<int,int>>> temp=make_pair(tmp,check);
    bfs.push(temp);
    while(bfs.size()){
        int size=bfs.size();
        for(int i=0;i<size;i++){
            temp=bfs.front();
            bfs.pop();
            temp.first.push_back(0);
            temp.second.push_back(make_pair(1,1));
            pair<vector<int>,vector<pair<int,int>>> swap;
            for(int j=1;j<=n;j++){
                swap=temp;
                swap.first[layer]=j;
                if(judge(swap.first,layer,swap.second)) {
                    bfs.push(swap);
                }
            }
        }
        layer++;
        //cout<<bfs.size()<<" ";
        if(layer==n){
            while(bfs.size()){
                ans.push_back(bfs.front().first);
                bfs.pop();
            }
            break;
        }
    }
}
bool numbers::judge(vector<int> stage,int idx,vector<pair<int,int>> &id){  //judge if current list is valid;
    for(int i=0;i<idx;i++){
        if(stage[i]==stage[idx]) return false;
        if(stage[i]<stage[idx]&&id[i].first>=id[idx].first) {
            id[idx].first=id[i].first+1;
            if(id[idx].first>shd1) {
                return false;
            }
        }
        if(stage[i]>stage[idx]&&id[i].second>=id[idx].second) {
            id[idx].second=id[idx].second+1;
            if(id[idx].second>shd2) {
                return false;
            }
        }
    }
    return true;
}
int main(){
    numbers *n=new numbers(9,3,3);
    n->find();
    n->show(); //output 100 possible answers;
}