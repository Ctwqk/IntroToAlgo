#include <iostream>
#include <vector>
#include <map>
using namespace std;
class DNAMatch{
    static const long long int MOD_VAL=39916801;
private:
    string pattern,target;
    long long int p=0;
    map<char,int> check;
    vector<int> matches;
    bool isMatch(int idx){
        int i=0;
        for(;i<pattern.size();i++) if(pattern[i]!=target[idx-pattern.size()+i]) break;
        return i==pattern.size();
    }
public :
    DNAMatch(string s1,string s2):pattern(s1),target(s2){
        check['A']=0;
        check['C']=1;
        check['G']=2;
        check['T']=3;
        for(char c:pattern) p=((p<<2)+check[c])%MOD_VAL;
    }
    vector<int> match(){
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
int main(){
    string target="TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATAACGTACGT";
    string pattern="ACTAGCTACTGTACT";
    DNAMatch *dnam=new DNAMatch(pattern,target);
    for(int i:dnam->match()) cout<<i<<" ";
}