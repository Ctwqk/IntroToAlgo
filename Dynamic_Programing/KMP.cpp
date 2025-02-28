#include <iostream>
#include <vector>
using namespace std;
class KMP{
private:
    string target;
    string pattern;
    int *lps,n,m;
    void getLPS(){         //calculating the prefix lengths;
        int len=0;
        for(int i=1;i<n;i++){
            if(pattern[i]==pattern[len])len++;
            else while(len&&pattern[i]!=pattern[len]) len=lps[len-1];
            lps[i]=len;
        }
        //for(int i=0;i<n;i++) cout<<lps[i]<<" ";
        //cout<<endl;
    }
public:
    KMP(string s1,string s2):target(s1),pattern(s2){
        n=s2.size(),m=s1.size();
        lps=new int[n]();
        getLPS();
    }
    vector<int> calculateKMP(){
        int i=0,j=0;
        vector<int> ans;
        while(j<m){
            if(target[j]==pattern[i]){
                if(++i==n)ans.push_back(j-n+1);
                j++;
            }
            else i>0?i=lps[i-1]:j++;
        }
        return ans;
    }
};
void testKMP(){
    string s1="TATTACTAGCTACTGTACTCGGCAGCCTATTCACTAGCTACTGTACTGGAGTACTATTCGGTGCGTATTCGACTAGCTACTGTACTGATAACGTACGT";
    string s2="ACTAGCTACTGTACT";
    KMP *k=new KMP(s1,s2);
    vector<int> ans=k->calculateKMP();
    for(int i=0;i<ans.size();i++) cout<<ans[i]<<" ";
}
int main(){
    testKMP();
}
