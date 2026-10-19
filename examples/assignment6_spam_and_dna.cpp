#include <iostream>
#include <vector>
#include <stack>
#include <map>
using namespace std;

class TrieNode{  //only a-z and A-Z supported, other input may cause undifined operation;
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
    //check whether incoming letter is valid, a-z or A-Z;
    int judge(char c){      
        int pos=c-'a';
        if(pos<0||pos>25) pos=c-'A';
        if(pos<0||pos>25) return -1;
        return pos;
    }
    //in case that when deleting some word, and such word turn out to not belong to trie, 
    //this function cancels any changes have been maded;
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
    bool insert(string word); //insert single string;
    bool inserts(vector<string> words); //insert a vector of strings;
    bool del(string word);  //delete a single string;
    bool dels(vector<string> word);  //delete a vector of strings;
    bool search(string s);  //search a single string;
    bool* searchs(vector<string> strings);  //search a vector of strings;
    bool change(string s,string tars);  //change a string in trie into another;
    void show();  //print all strings in trie;
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
bool TrieNode::del(string s){  
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
    cout<<"In Trie: "<<endl;
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
    string message="I am asda, u r asd, please bqrqwf to VASVE, so that u efwd a dasegawggdav";
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
    root->inserts(K);
    root->show();
    for(string s:wordInMessage){
        if(root->search(s)){
            cout<<"decteted: "<<s<<endl;
        }
    }

}

                                    /********below is problem 4**********/

class RKMatch{
    static const long long int MOD_VAL=39916801;  //large prime number as MOD_VAL;
private:
    string pattern,target;
    long long int p=0;
    map<char,int> check;  //map ACGT to 0123, an array should also words;
    vector<int> matches;
    bool isMatch(int idx){
        int i=0;
        for(;i<pattern.size();i++) if(pattern[i]!=target[idx-pattern.size()+i]) break;
        return i==pattern.size();
    }
public :
    RKMatch(string s1,string s2):pattern(s1),target(s2){
        check['A']=0;
        check['C']=1;
        check['G']=2;
        check['T']=3;
        for(char c:pattern) p=((p<<2)+check[c])%MOD_VAL;
    }
    vector<int> match(){  //use binary to simulate quaternary;
        long long int tmp=0;
        int idx=0;
        for(;idx<pattern.size();idx++) tmp=((tmp<<2)+check[target[idx]])%MOD_VAL;
        if(tmp==p&&isMatch(idx))matches.push_back(idx-pattern.size());
        while(idx<target.size()){
            long long int swa=(check[target[idx-pattern.size()]]<<(2*pattern.size()-2))%MOD_VAL;
            tmp=(((tmp-swa+MOD_VAL)%MOD_VAL)<<2)%MOD_VAL+check[target[idx++]]%MOD_VAL;
            if(tmp==p&&isMatch(idx)) matches.push_back(idx-pattern.size());
        }
        return matches;
    }
};
void testDNAMatch(){
    string target="TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATAACGTACGT";
    string pattern="ACTAGCTACTGTACT";
    RKMatch *dnam=new RKMatch(pattern,target);
    cout<<"target: "<<target<<endl;
    cout<<"pattern: "<<pattern<<endl;
    cout<<"Pattern appears in following positions: ";
    for(int i:dnam->match()) cout<<i<<" ";
}
int main(){
    cout<<"SpamDetect: "<<endl;
    detectSpam();
    cout<<endl;
    cout<<"DNAMatch: "<<endl;
    testDNAMatch();
}