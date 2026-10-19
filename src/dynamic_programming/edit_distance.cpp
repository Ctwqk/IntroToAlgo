#include <iostream>
#include <vector>
using namespace std;
class ED{
private:
    string x;
    string y;

public:
    ED(string s1,string s2):x(s2),y(s2){

    }
    int calculate(){
        int n=x.size(),m=y.size();
        int **dp=new int*[n+1];
        for(int i=0;i<=n;i++) {
            dp[i]=new int[m+1]();
            dp[i][0]=i;
        }
        for(int i=0;i<=m;i++) dp[0][i]=i;
        for(int i=1;i<=n;i++){
            for(int j=1;j<=m;j++){
                dp[i][j]=min(dp[i-1][j-1],min(dp[i-1][j],dp[i][j-1]))+1;
            }
        }
        return dp[n][m];
    }
};