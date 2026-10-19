#include <iostream>
#include <vector>
#include <stack>
#include <map>
using namespace std;

class TrieNode{  //tring some trie;
public:
private:
    TrieNode **children;
    int *labels;
    int val;
    bool isEnd;
    bool visit;
    char decode(int val){
        if(val==-1) return '#';
        if(val<26) return 'a'+val;
        if(val<52&&val>=26) return 'A'+val-26;
        return '!';
    }
    int judge(char c){
        int pos=c-'a';
       // cout<<c<<" "<<pos<<endl;
        if(pos<0||pos>25) pos=c-'A';
        if(pos<0||pos>25) return -1;
        return pos;
    }
    bool makeUp(stack<TrieNode*> dfs,stack<int> poss){
        while(dfs.top()){
            dfs.top()->labels[poss.top()]--;
            dfs.pop();
            poss.pop();
        }
        return true;
    }
public:
    TrieNode():val(-1),isEnd(false),visit(false){
        labels=new int[52]();
    }
    TrieNode(int val, bool ie=false):val(val),isEnd(ie),visit(false){
        children=new TrieNode*[52];
        labels=new int[52]();
    }
    bool insert(string word);
    bool inserts(vector<string> words);
    bool del(string word);
    bool dels(vector<string> word);
    bool search(string s);
    bool* searchs(vector<string> strings);
    bool change(string s,string tars);
    void show();
};
bool TrieNode::change(string s,string tars){
    return this->del(s)&&this->insert(tars);
}
bool TrieNode::inserts(vector<string> strings){
    bool flag=true;
    for(string s:strings)flag!=insert(s);
    return flag;
}
bool TrieNode::insert(string word){
    if(this->val!=-1) cout<<"inserting at a non root node, think twice"<<endl;
    TrieNode *p=this;
    int pos;
    for(char c:word){
        pos=judge(c);
        if(pos==-1) return false;
        p->labels[pos]++;
        if(p->labels[pos]<=1) p->children[pos]=new TrieNode(pos);
        p=p->children[pos];
    }
    p->isEnd=true;
    return true;
}
bool TrieNode::dels(vector<string> strings){
    bool flag=true;
    for(string s:strings)flag&=del(s);
    return flag;
}
bool TrieNode::del(string s){  //not tested
    if(this->val!=-1) cout<<"deleting at a non root node, think twice"<<endl;
    if(!search(s)) return false;
    TrieNode *p=this;
    int pos;
    stack<TrieNode*> dfs;
    stack<int> poss;
    for(char c:s){
        pos=judge(c);
        if(pos==-1) makeUp(dfs,poss);
        poss.push(pos);
        dfs.push(p);
        p->labels[pos]--;
        p=p->children[pos];      
    }
    p->isEnd=false;
}
bool* TrieNode::searchs(vector<string> strings){
    int n=strings.size();
    bool* ans=new bool[n];
    for(int i=0;i<n;i++){
        ans[i]=search(strings[i]);
    }
    return ans;
}
bool TrieNode::search(string s){
    TrieNode *p=this;
    int pos;
    for(char c:s){
        pos=judge(c);
        if(p->labels[pos]<=0) return false;
        p=p->children[pos];
    }
    return p->isEnd;
}
void TrieNode::show(){
    stack<TrieNode*> dfs;
    string ans;
    TrieNode* p;
    dfs.push(this);
    int flag=0;
    while(dfs.size()){
        p=dfs.top();
        flag=0;
        for(int i=0;i<52;i++){
            if(p->labels[i]>0&&!p->children[i]->visit){
                dfs.push(p->children[i]);
                ans=ans+decode(i);
                flag=1;
                break;
            }
        }
        if(!flag) {
            dfs.pop();
            if(p->isEnd) cout<<ans<<endl;
            p->visit=true;
            ans=ans.substr(0,ans.size()-1);
        }
    }
}

void detectSpam(){
    vector<string> K{"asda","aebrq","bqrqwf","VASVE","vasdawdf","dasegawggdav","efwd","asd","davcase","vwdfd","davsef"};
    string message="I am asda, u r asd, please bqrqwf to VASAE, so that u efwd a dasegawggdav";
    vector<string> wordInMessage;
    while(true){
        size_t pos=message.find(' ');
        if(pos==string::npos) break;
        wordInMessage.push_back(message.substr(0,pos));
        message=message.substr(pos+1,message.size()-pos);
        string s=wordInMessage.back();
        char c=s.back();
        if(c<65||(c>90&&c<97)||c>122) wordInMessage.back()=s.substr(0,s.size()-1);
    }
    wordInMessage.push_back(message);

    TrieNode* root=new TrieNode(-1);
    for (string s:K){
        root->insert(s);
    }

    for(string s:wordInMessage){
        if(root->search(s)){
            cout<<"decteted: "<<s<<endl;
        }
    }

}


int main(){
    detectSpam();
}