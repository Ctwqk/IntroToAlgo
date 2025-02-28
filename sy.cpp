#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <stack>
#include <tuple>
#include <math.h>
using namespace std;
class maximum_num_houses_solar{
private:
    int *numOfSolarPanels;
    int *timeOfPackageCanBeUsed;
    int *minimumHouseNeeded;
    int n,m;
public:
    maximum_num_houses_solar(int *solarPanel,int* packageTime,int* houseMin,int n,int m):minimumHouseNeeded(houseMin),
                                numOfSolarPanels(solarPanel),timeOfPackageCanBeUsed(packageTime),n(n),m(m){}
    int mac_num_house(){
        priority_queue<int,vector<int>,greater<int>>houses;
        for(int i=0;i<m;i++) houses.push(minimumHouseNeeded[i]);
        vector<pair<int,int>> packages;
        for(int i=0;i<n;i++){
            packages.push_back(make_pair(numOfSolarPanels[i],timeOfPackageCanBeUsed[i]));
        }
        sort(packages.begin(),packages.end());
        int ans=0;
        while(houses.size()){
            int tmp=houses.top();
            //cout<<tmp<<endl;
            houses.pop();
            int i=0;
            for(;i<n;i++){
                if(tmp<=packages[i].first&&packages[i].second>0){
                    packages[i].second--;
                    break;
                }
            }
            if(i<n) ans++;
            else break;
        }
        return ans;
    }
};

bool isPrime(int x){
    cout<<INT_MAX;
}
int main(){
    int i=0;
    for(;i<3;i++){
        if(i==2) break;
    }
    cout<<i;
}